#ifndef _LOG_H_
#define _LOG_H_

#include <stdio.h>
#include <QString>
#include "logtypes.h"

#ifdef _WIN32
#pragma warning(disable:4512)
#endif

#define ENABLE_LOGGING
#ifdef ENABLE_LOGGING

#define LOG_MSG( ClassLogger, Type, Msg )   { if ( (ClassLogger).WillLog(Type) ) (ClassLogger).LogMsg( Type, __FILE__, __LINE__, Msg ); }


class Logger
{
private:
    QString m_sModule;
    QString m_sComponent;
public:
    Logger( const QString &sModule, const QString &sComponent );
    void LogMsg( LogTypes::LogTypes type, const char *sFile, int nLine, const QString &sMsg );
    bool WillLog(LogTypes::LogTypes type);
};

#else

#define LOG_MSG( ClassLogger, Type, Msg )

#endif



#endif
