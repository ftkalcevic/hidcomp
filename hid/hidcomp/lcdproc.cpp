#include "stdafx.h"
#include "lcdproc.h"

const char *sOK = "OK\n";
const char *sError = "?\n";
const int MS_WAIT = 200;

LCDProc::LCDProc( int nPort )
: m_Logger(QCoreApplication::applicationName(), "LCDProc")
, m_nPort( nPort )
, m_tcpServer( NULL )
{
}

LCDProc::~LCDProc(void)
{
}

void LCDProc::queueMessage( LCDCmd *pCmd )
{
    QMutexLocker lock( &m_BufferMutex );
    m_Buffer.push_back( pCmd );
}

LCDCmd *LCDProc::getQueueMessage()
{
    QMutexLocker lock( &m_BufferMutex );
    if ( !m_Buffer.isEmpty() )
	return m_Buffer.takeFirst();
    else
	return NULL;
}

int LCDProc::queueLength() const
{
    return m_Buffer.length();
}


const QString LCDProc::ProcessCommand( QString sLine )
{
    LOG_DEBUG( m_Logger, QString("Got LCD CMD: ")+sLine );

    sLine = sLine.trimmed();
    QStringList splitLine = sLine.split(" " );
    if ( splitLine.count() == 0 )
	return sError;

    QStringList Args;
    int index = sLine.indexOf( " " );
    if ( index >= 0 )
    {
	QString sArgs = sLine.mid(index).trimmed();
	Args = sArgs.split(",");
	for ( int i = 0; i < Args.count(); i++ )
	{
	    QString s = Args[i];
	    s = s.trimmed();
	    Args[i] = s;
    	}
    }

    QString sCmd = splitLine[0].toLower();
    if ( sCmd == "hello" )
    {
	// nothing to do?  Reset?
    }
    else if ( sCmd == "cls" )
    {
	queueMessage( new LCDClearScreen() );
    }
    else if ( sCmd == "bg" || sCmd == "fg" )
    {
	if ( Args.count() != 3 )
	    return sError;
	int rgb[3];
	bool bOk;
	for ( int i = 0; i < 3; i++ )
	{
	    rgb[i] = Args[i].toInt(&bOk);
	    if ( !bOk )
		return sError;
	}
	if ( sCmd == "bg" )
	    queueMessage( new LCDSetBackground( rgb[0], rgb[1], rgb[2] ) );
	else
	    queueMessage( new LCDSetForeground( rgb[0], rgb[1], rgb[2] ) );
    }
    else if ( sCmd == "text" || sCmd == "textp" )
    {
	if ( Args.count() < 3 )	// if there is a comma in the string, split will split it, so we need at least 3 args
	    return sError;
	int x,y;
	bool bOk;
	x = Args[0].toInt(&bOk);
	if ( !bOk )
	    return sError;
	y = Args[1].toInt(&bOk);
	if ( !bOk )
	    return sError;
	bool pixel = false;
	if ( sCmd == "textp" )
	    pixel = true;
	// find the second comma.  Text starts after that.
	index = 0;
	index = sLine.indexOf(",",index+1);
	index = sLine.indexOf(",",index+1);
	QString s = sLine.mid(index+1);
	s = s.trimmed();
	if ( s.startsWith("\"") && s.endsWith("\"") )
	    s = s.mid(1,s.length()-2);
	queueMessage( new LCDText( x, y, s, pixel ) );
    }

    else if ( sCmd == "font" )
    {
	if ( Args.count() != 1 )
	    return sError;
	int font;
	bool bOk;
	font = Args[0].toInt(&bOk);
	if ( !bOk )
	    return sError;
	queueMessage( new LCDSetFont( font ) );
    }

    else if ( sCmd == "backlight" )
    {
	if ( Args.count() != 1 )
	    return sError;
	int intensity;
	bool bOk;
	intensity = Args[0].toInt(&bOk);
	if ( !bOk )
	    return sError;
	queueMessage( new LCDSetBacklight( intensity ) );
    }

    else if ( sCmd == "rect" || sCmd == "fill" )
    {
	if ( Args.count() != 4 )
	    return sError;
	int coord[4];
	bool bOk;
	for ( int i = 0; i < 4; i++ )
	{
	    coord[i] = Args[i].toInt(&bOk);
	    if ( !bOk )
		return sError;
	}
	bool fill = false;
	if ( sCmd == "fill" )
	    fill = true;
	queueMessage( new LCDRectangle( coord[0], coord[1], coord[2], coord[3], fill ) );
    }
    else
	return sError;

    return sOK;
}

bool LCDProc::Initialise()
{
    LOG_DEBUG( m_Logger, "Initialising LCDProc" );

    // Create a listening socket.  We check we can listen, and return false if we can't.
    // Then we close the socket, spawn the thread, and start listening again in th
    // thread.  If we don't Qt complains about children of different threads.  So we play nice.
    m_tcpServer = new QTcpServer();

    if ( !m_tcpServer->listen( QHostAddress::Any, (quint16)m_nPort ) )
    {
	LOG_ERROR( m_Logger, QString("Failed to open LCD listen port %1: %2").arg(m_nPort).arg(m_tcpServer->errorString()) );
	delete m_tcpServer;
	m_tcpServer = NULL;
	return false;
    }

    // Spawn socket thread
    m_tcpServer->close();
    delete m_tcpServer;

    start();

    return true;
}


void LCDProc::run()
{
    m_bRun = true;
    m_tcpServer = new QTcpServer();

    // We only accept one connection at a time.
    while ( m_bRun )
    {
	// set socket to listen
	if ( m_bRun && !m_tcpServer->listen( QHostAddress::Any, (quint16)m_nPort ) )
	{
	    LOG_ERROR( m_Logger, QString("Failed to reopen LCD listen port %1: %2").arg(m_nPort).arg(m_tcpServer->errorString()) );
	    m_bRun = false;
	}

	// wait for connections
	while ( m_bRun )
	{
	    m_tcpServer->waitForNewConnection( MS_WAIT );

	    if ( !m_bRun )
		break;

	    if ( m_tcpServer->hasPendingConnections() )
		break;
	}
        
	// handle connection
	if ( m_bRun )
	{
	    QTcpSocket *pSocket = m_tcpServer->nextPendingConnection();
	    m_tcpServer->close();

	    QString sBuffer;
	    while ( m_bRun )
	    {
		if ( pSocket->waitForReadyRead( MS_WAIT ) )
		{
		    // Process one text line at a time.  Do this by hand so we don't block.
		    while ( pSocket->bytesAvailable() > 0 )
		    {
			char c;
			if ( !pSocket->getChar( &c ) )
			    break;

			if ( c == '\n' || c == '\r' )
			{
			    if ( sBuffer.length() > 0 )
			    {
				QString sRet = ProcessCommand( sBuffer );
				pSocket->write( sRet.toAscii().constData() );
				sBuffer.clear();
			    }
			}
			else
			    sBuffer += c;
		    }
		}

		if ( pSocket->state() == QAbstractSocket::UnconnectedState || 
		     pSocket->state() == QAbstractSocket::ClosingState )
		{
		    // Socket problem maybe
		    LOG_ERROR( m_Logger, "LCD Socket closed" );
		    break;
		}

		if ( !m_bRun )
		    break;
	    }

	    pSocket->close();
	    delete pSocket;
	}

    }

    m_tcpServer->close();
}


/*
hello
    OK
cls
    OK
bg r,g,b
    OK
fg r,g,b
    OK
text x,y,"string"
    OK
textp x,y,"string"
    OK
font n
    OK
backlight n
    OK
rect x,y,w,h
    OK
fill x,y,w,h
    OK
*/

