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

#ifndef _EMCLCDITEM_H_
#define _EMCLCDITEM_H_

#include "emchiditem.h"

class EMCLCDItem
{
public:
    ELCDDisplayData::ELCDDisplayData m_eData;   // The data that is to be displayed
    int m_nIndex;				// The index of the data item, eg Position[Axis0]
    int m_nRow;
    int m_nCol;
    QByteArray m_sFormat;			// Format String - use QByteArray because we work with char *
    double m_dScale;
    QString m_sLookupDefault;			// Default if nothing in lookup
    QMap<int,QString> m_Lookup;			// look up table to %b enum format
    int m_nPinIndex;				// for user types, the index to the input pin in the pin structure
    bool m_bLastBool;
    double m_dLastFloat;
    unsigned int m_nLastU32;
    int m_nLastS32;
    QString m_sLastString;
    QString m_sLastFormattedData;		// Formatted version of the last value

    void ProcessFormatString( LCDEntry *pLCDEntry );
    void Init();

    bool CheckDataChange(std::vector<EMCPin> &Pins, bool bForceUpdate );
    QString FormatData( int n );
    QString FormatData( unsigned int n );
    QString FormatData( double d );
    QString FormatData( bool b );
    QString FormatData( const QString &s );
    QString FormatEnum( int n );
};

class EMCLCDPage
{
public:
    ~EMCLCDPage()
    {
	for ( int i = 0; i < m_Items.count(); i++ )
	    delete m_Items[i];
	m_Items.clear();
    }

    QList<EMCLCDItem *> m_Items;
};

#endif
