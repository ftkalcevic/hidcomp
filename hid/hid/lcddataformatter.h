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
