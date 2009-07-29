#ifndef _HIDDEVICES_H_
#define _HIDDEVICES_H_

#include "hiddevice.h"

class HIDDevices
{
public:
    HIDDevices();
    ~HIDDevices(void);
    static bool Open(int nDebugLevel=0);

private:
    static bool m_bOpen;

public:
    std::vector<HIDDevice *>  m_Devices;
    bool FindHIDDevices();
    std::vector<HIDDevice *> SearchHIDDevices( bool bPID, unsigned short nPID, bool bVID, unsigned short nVID, bool bManufacturer, const QString &sManfacturer, bool bProduct, const QString &sProduct, bool bSN, const QString &sSerialNumber, bool bSystemId, const QString &sSystemId );
    Logger m_Logger;
};

#endif
