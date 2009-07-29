#ifndef _LCDDATAFORMATTER_H_
#define _LCDDATAFORMATTER_H_

#ifdef _WIN32
#pragma warning(push, 1)
#endif
#include <QString>
#include <QByteArray>
#include <QMap>
#ifdef _WIN32
#pragma warning(pop)
#pragma warning(disable:4251)
#endif


class LCDDataFormatter
{
public:
    // newFormatString is a byte array because it is designed to be used with printf which requires a char *
    static bool ProcessFormatString( const QString &sFormatString, QByteArray &newFormatString, QString &sDefaultString, QMap<int,QString> &lookupTable );
    static QString snprintf( const char *sFmt, ... );
    static QString vsnprintf( const char *sFmt, va_list args );
};

#endif
