#include "log.h"
#include "logcore.h"

// Logger implementation

Logger::Logger( const QString &sModule, const QString &sComponent )
: m_sModule( sModule )
, m_sComponent( sComponent )
{
    LogCore::Init();
}

void Logger::LogMsg( LogTypes::LogTypes type, const char *sFile, int nLine, const QString &sMsg )
{
    QDateTime now = QDateTime::currentDateTime();
    if ( WillLog(type) )
        LogCore::LogMsg( type, now, m_sModule, m_sComponent, sFile, nLine, sMsg );
}

bool Logger::WillLog(LogTypes::LogTypes type)
{
    return LogCore::WillLog( m_sModule, m_sComponent, type );
}
