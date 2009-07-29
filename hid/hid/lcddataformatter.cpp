#include "lcddataformatter.h"
#include "common.h"

#include <QRegExp>
#include <QMutex>
#include <QMutexLocker>

#ifdef _WIN32

#include <windows.h> // for EXCEPTION_ACCESS_VIOLATION 
#include <excpt.h> 

#else

#include <signal.h>
#include <setjmp.h>

#endif

#define	MAX_LINE_WIDTH	    128	    // some arbitrary number.  LCDs usually aren't more than 40 wide.


bool LCDDataFormatter::ProcessFormatString( const QString &sFormatString, QByteArray &newFormatString, QString &sDefaultString, QMap<int,QString> &lookupTable )
{
    // look for %[-][n][.][n]:[n,str:]b
    QRegExp exp( ":(\\d+,[^:]+:)+b" );

    QString sNewFormat = sFormatString;
    int pos = exp.indexIn(sFormatString, 0);
    if (pos >= 0) 
    {
	// a match.  this is a %b enum formatter.  Replace %...b with %s and extract the enums
        QString sList = sFormatString.mid( pos+1, exp.matchedLength() - 2 );
	sNewFormat = sFormatString;
        sNewFormat.replace( pos, exp.matchedLength(), "s" );

        // Extract the lists
        QRegExp exp2("(\\d+),([^:]+):");
        pos = 0;
        while ( pos >= 0 )
        {
            pos = exp2.indexIn(sList, pos);
            if ( pos >= 0 )
            {
                int nIndex = exp2.cap(1).toInt();
                sDefaultString = exp2.cap(2);
                lookupTable.insert( nIndex, sDefaultString );
                pos += exp2.matchedLength();
            }
        }
    }
    else
    {
	sNewFormat = sFormatString;
    }

    newFormatString = sNewFormat.toAscii();

    return true;
}


#ifdef _WIN32

int exception_filter(unsigned int code, struct _EXCEPTION_POINTERS *ep) 
{ 
    // we only protect from access violations
    if ( code == EXCEPTION_ACCESS_VIOLATION ) 
	return EXCEPTION_EXECUTE_HANDLER; 
    else 
	return EXCEPTION_CONTINUE_SEARCH; 
} 

static void DoSnprintf( char *buffer, int nLen, const char *sFmt, va_list args )
{
    __try
    {
	::vsnprintf( buffer, nLen, sFmt, args);
    }
    __except( exception_filter(GetExceptionCode(), GetExceptionInformation()) )
    {
	// catch any serious exceptions
	strcpy( buffer, "E?????" );
    }
}

#else

static jmp_buf catch_exception;
static QMutex mutex;
static void handle_signal( int )
{
    siglongjmp( catch_exception, 1 );
}

static void DoSnprintf( char *buffer, int nLen, const char *sFmt, va_list args )
{
    QMutexLocker lock( &mutex );

    sighandler_t oldSegv = signal( SIGSEGV, handle_signal );
    sighandler_t oldBus = signal( SIGBUS, handle_signal );

    if ( sigsetjmp( catch_exception, 1 ) == 0 )
	::vsnprintf( buffer, nLen, sFmt, args);
    else
	strcpy( buffer, "E?????" );

    signal( SIGSEGV, oldSegv );
    signal( SIGBUS, oldBus );
}


#endif


QString LCDDataFormatter::snprintf( const char *sFmt, ... )
{
    va_list args;
    va_start(args, sFmt);

    return LCDDataFormatter::vsnprintf( sFmt, args );
}

QString LCDDataFormatter::vsnprintf( const char *sFmt, va_list args )
{
    char buffer[MAX_LINE_WIDTH];
    buffer[0] = 0;
    buffer[MAX_LINE_WIDTH-1] = 0;   // set last byte too, as vsnprintf wont set it if the output is too big for the buffer

    DoSnprintf( buffer, countof(buffer)-1, sFmt, args );

    return QString( buffer );
}
