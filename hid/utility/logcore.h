#ifndef _LOGCORE_H_
#define _LOGCORE_H_

#include <QMutex>
#include <QMutexLocker>
#include <QDateTime>
#include <QMap>
#include <QThread>

#include "logtypes.h"

class LogCore
{
    static QMutex m_LogMutex;
    static int m_nRefCount;
    static LogCore *m_LogCore;
    static bool m_bLog;
    static QMap<QThread *,int> m_ThreadMap;

    static int GetThreadId();

public:
    LogCore();
    static void SetLog( bool b ) { m_bLog = b; }
    static void Init();
    static void LogMsg( LogTypes::LogTypes type, QDateTime timeStamp, const QString &sModule, const QString &sComponent, const QString &sFile, int nLine, const QString &sMsg );
    static bool WillLog( const QString &sModule, const QString sComponent, LogTypes::LogTypes type );
};


#endif
