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

#ifndef __EMCPIN_H__
#define __EMCPIN_H__

#include <QString>
#include "hal.h"

struct EMCPin
{
    EMCPin( const QString &sName, hal_type_t type, hal_pin_dir_t dir )
    {
        sPinName = sName;
        nHalType = type;
        nHalDir = dir;
        pData = NULL;
    }

    QString sPinName;
    hal_type_t nHalType;
    hal_pin_dir_t nHalDir;
    void **pData;
};

#endif
