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

#ifndef _EMCINTERFACE_H_
#define _EMCINTERFACE_H_

#include "emc.hh"
#include "emc_nml.hh"

#include "lcddisplaydata.h"

class EmcInterface
{
public:
    EmcInterface();
    ~EmcInterface(void);

    bool Initialise( int argc, char *argv[] );
    bool Update();
    void Close();
    QString GetStringData( ELCDDisplayData::ELCDDisplayData eData, int nIndex );
    int GetIntData( ELCDDisplayData::ELCDDisplayData eData, int nIndex );
    double GetFloatData( ELCDDisplayData::ELCDDisplayData eData, int nIndex );

private:
    bool iniLoad(const char *filename);
    bool tryNml();
    bool emcTaskNmlGet();
    bool emcErrorNmlGet();
    bool updateError();
    bool updateStatus();

    Logger m_Logger;
    bool m_bEmcDebug;
    QString m_sIniFile;

    // the NML channels to the EMC task
    RCS_CMD_CHANNEL *m_emcCommandBuffer;
    RCS_STAT_CHANNEL *m_emcStatusBuffer;
    EMC_STAT *m_emcStatus;
    // the NML channel for errors
    NML *m_emcErrorBuffer;

    QString m_sErrorString;
    QString m_sOperatorTextString;
    QString m_sOperatorDisplayString;
    QString m_sLastMsg;
};

extern EmcInterface *emcIFace;

#endif
