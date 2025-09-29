// hidcomp/hidconfig, HID device interface for emc
// Copyright (C) 2008, Frank Tkalcevic, www.franksworkshop.com

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.

#include "lcddataformatter.h"
#include "common.h"

#include <QRegularExpression>
#include <QMutex>
#include <QMutexLocker>

#ifdef _WIN32

#include <windows.h> // for EXCEPTION_ACCESS_VIOLATION 
#include <excpt.h> 

#else

#include <signal.h>
#include <setjmp.h>
#include <stdio.h>
#include <stdarg.h>
#endif

#define	MAX_LINE_WIDTH	    128	    // some arbitrary number.  LCDs usually aren't more than 40 wide.

static bool isOctal( QChar c, int &n )
{
    if ( c.isDigit() && c >= QChar('0') && c <= QChar('7') )
    {
	n = c.toLatin1() - '0';
	return true;
    }
    return false;
}

static bool isHex( QChar c, int &n )
{
    if ( c >= QChar('0') && c <= QChar('9') )
    {
	n = c.toLatin1() - '0';
	return true;
    }
    else
    {
	c = c.toUpper();
	if ( c >= QChar('A') && c <= QChar('F') )
	{
	    n = c.toLatin1() - 'A' + 10;
	    return true;
	}
    }
    return false;
}

static QString ProcessEscapeSequences( const QString &sFormatString )
{
    QString sRet;
    int nLen = sFormatString.length();
    for ( int i = 0; i < sFormatString.length(); i++ )
    {
	QChar c = sFormatString[i];
	if ( c == '\\' )
	{
	    i++;
	    if ( i < nLen )
	    {
		int n;
		c = sFormatString[i];
		switch ( c.toLatin1() )
		{
		    case 'a': sRet += QChar('\a'); continue;
		    case 'b': sRet += QChar('\b'); continue;
		    case 'f': sRet += QChar('\f'); continue;
		    case 'n': sRet += QChar('\n'); continue;
		    case 'r': sRet += QChar('\r'); continue;
		    case 't': sRet += QChar('\t'); continue;
		    case 'v': sRet += QChar('\v'); continue;
		    default:
			if ( c == 'x' )
			{
			    QChar nBadChar = (QChar)0;
			    int nValue = 0;
			    for ( int j = 0; j < 2; j++ )
			    {
				i++;
				if ( i < nLen )
				{
				    c = sFormatString[i];
				    if ( isHex(c,n) )
					nValue = (nValue << 4) | n;
				    else
				    {
					nBadChar = c;
					break;
				    }
				}
				else
				    break;
			    }
			    sRet += QChar( nValue );
			    if ( nBadChar != 0 )
				sRet += nBadChar;
			}
			else if ( isOctal(c,n) )
			{
			    QChar nBadChar = (QChar)0;
			    int nValue = n;
			    for ( int j = 0; j < 2; j++ )
			    {
				i++;
				if ( i < nLen )
				{
				    c = sFormatString[i];
				    if ( isOctal(c,n) )
					nValue = (nValue << 3) | n;
				    else
				    {
					nBadChar = c;
					break;
				    }
				}
				else
				    break;
			    }
			    sRet += QChar( nValue );
			    if ( nBadChar != 0 )
				sRet += nBadChar;
			}
			else
			{
			    sRet += c;
			}
			continue;
		}
	    }
	}
	else
	    sRet += c;
	 
    }
    return sRet;
}

bool LCDDataFormatter::ProcessFormatString( const QString &sFormatString, QByteArray &newFormatString, QString &sDefaultString, QMap<int,QString> &lookupTable )
{
    QString sNewFormat = ProcessEscapeSequences( sFormatString );

    // look for %[-][n][.][n]:[n,str:]b
    QRegularExpression exp( ":(\\d+,[^:]+:)+b" );
    QRegularExpressionMatch match = exp.match(sNewFormat);

    int pos = match.capturedStart(0);
    if (pos >= 0) 
    {
	// a match.  this is a %b enum formatter.  Replace %...b with %s and extract the enums
        QString sList = sNewFormat.mid( pos+1, match.capturedLength(0) - 2 );
	sNewFormat = sNewFormat;
        sNewFormat.replace( pos, match.capturedLength(0), "s" );

        // Extract the lists
        QRegularExpression exp2("(\\d+),([^:]+):");
        QRegularExpressionMatch match2 = exp2.match(sList);

        for ( int i = 0; match2.hasCaptured(i); i++ )
        {
            pos = match2.capturedStart(i);
            if ( pos >= 0 )
            {
                int nIndex = match2.capturedView(1).toInt();
                sDefaultString = match2.capturedView(2).toString();
                lookupTable.insert( nIndex, sDefaultString );
            }
        }
    }

    newFormatString = sNewFormat.toLatin1();

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
