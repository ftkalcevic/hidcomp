#ifndef _HIDDEVICETHREAD_H_
#define _HIDDEVICETHREAD_H_

#ifdef _WIN32
#pragma warning(push, 1)
#endif

#include <QString>
#include <QThread>
#include <QVector>
#include <QMutex>
#include <QMutexLocker>

#ifdef _WIN32
#pragma warning(pop)
#pragma warning(disable:4251)
#endif

#include "hidparser.h"
#include "hiddevice.h"

class HIDDevice;

class HIDDeviceThread: public QThread
{
public:
    HIDDeviceThread( QMutex &SendBufferMutex, QList<QVector<byte> > &SendBuffer, QMutex &ReceiveBufferMutex, QList<QVector<byte> > &ReceiveBuffer, int nLongestInReport, int nLongestOutReport, HIDDevice *pDevice );
    ~HIDDeviceThread(void);

    void Signal();
    void Stop();

private:
    virtual void run();
    void ReadCallback(struct libusb_transfer *transfer);
    static void _ReadCallback(struct libusb_transfer *transfer);
    static void FDRemoved(int fd, void *user_data);
    static void FDAdded(int fd, short events, void *user_data);

    QMutex &m_SendBufferMutex;
    QList<QVector<byte> > &m_SendBuffer;
    QMutex &m_ReceiveBufferMutex;
    QList<QVector<byte> > &m_ReceiveBuffer;
    volatile bool m_bRunning;
    int m_nLongestInReport;
    int m_nLongestOutReport;
    HIDDevice *m_pDevice;
    Logger m_Logger;
    static void _WriteCallback(struct libusb_transfer *transfer);
    void WriteCallback(struct libusb_transfer *transfer);
    bool m_bSendBufferFree;
    bool m_bRebuild_fds;
    int m_nReadPipe;
    int m_nWritePipe;

    QList<struct libusb_transfer *> m_TransferBuffers;
};

#endif
