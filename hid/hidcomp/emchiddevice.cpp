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

#include "stdafx.h"
#include "log.h"
#include "emchiddevice.h"


static bool checkDeviceItemConfig( HIDItem *pCfgItem, HID_ReportItem_t *pReportItem, QString &sError )
{
    sError.clear();
    //	if ( pCfgItem->type() != type )
    //		sMismatch += QString("Incorrect type.  Expected '%1' got '%2'\n").arg(pBaseItem->type()).arg(type);
    if ( pCfgItem->nUsagePage != pReportItem->Attributes.UsagePage )
	sError += QString("Incorrect usage page.  Expected '%1' got '%2'\n").arg(pCfgItem->nUsagePage).arg(pReportItem->Attributes.UsagePage);

    if ( pCfgItem->nUsage != pReportItem->Attributes.Usage )
        sError += QString("Incorrect usage.  Expected '%1' got '%2'\n").arg(pCfgItem->nUsage).arg(pReportItem->Attributes.Usage);

    if ( sError.isEmpty() )
	return true;
    else
	return false;
}



EMCHIDDevice::EMCHIDDevice( const QString sModuleName, const QString sPinPrefix, const QString sFilename )
: m_Logger(QCoreApplication::applicationName(),"EMCHIDDevice")
, m_pHidCfg( NULL )
, m_pDevice( NULL )
{
    m_pDevice = NULL;
    m_bRun = false;
    m_sModuleName = sModuleName;
    m_sFileName = sFilename;

    m_pHidCfg = HID::CreateFromXML( sFilename );
    if ( m_pHidCfg == NULL )
    {
        LOG_MSG( m_Logger, LogTypes::Error, QString("Failed to process xml configuration file '%1'.").arg(sFilename) );
        throw;
    }

    // find the hid device
    HIDDevices Devices;
    std::vector<HIDDevice *> DeviceList = Devices.SearchHIDDevices( m_pHidCfg->criteria->bPID, m_pHidCfg->criteria->nPID, m_pHidCfg->criteria->bVID, m_pHidCfg->criteria->nVID, m_pHidCfg->criteria->bManufacturer, m_pHidCfg->criteria->sManufacturer, m_pHidCfg->criteria->bProduct, m_pHidCfg->criteria->sProduct, m_pHidCfg->criteria->bSerialNumber, m_pHidCfg->criteria->sSerialNumber, m_pHidCfg->criteria->bSystemId, m_pHidCfg->criteria->sSystemId );
    if ( DeviceList.size() != 1 )
    {
        LOG_MSG( m_Logger, LogTypes::Error, QString("Matched %1 USB HID devcies with the given criteria.  There must be exactly 1 match.").arg((uint)DeviceList.size()) );
        return;
    }
    m_pDevice = DeviceList[0];
    m_pDevice->PreprocessReportData();

    // todo - error check index for item (range) and lcd (range,type)
    // build a collection of items
    for ( unsigned int i = 0; i < m_pHidCfg->items.size(); i++ )
    {
	EMCHIDItem *obj = NULL;
        HIDItem *pCfgItem = m_pHidCfg->items[i];
	if ( pCfgItem->type == HIDItemType::LCD )
	{
	    // Find the alpha display collection
	    if ( pCfgItem->bEnabled )
	    {
    		obj = new EMCHIDLCD( sPinPrefix, pCfgItem, m_pDevice->ReportInfo().Collections[pCfgItem->nIndex] ); 
	    }
	}
	else
	{
	    HID_ReportItem_t *pDeviceItem = m_pDevice->ReportInfo().ReportItems[pCfgItem->nIndex];

	    if ( pCfgItem->bEnabled )
	    {
		// Make sure the items on the device match the configuration
		QString  sError;
		if ( !checkDeviceItemConfig( pCfgItem, pDeviceItem, sError ) )
		{
		    LOG_MSG( m_Logger, LogTypes::Error, QString("There was a problem matching up item index %1 when reloading the configuration.\n").arg(pCfgItem->nIndex) );
		    LOG_MSG( m_Logger, LogTypes::Error, QString("This means the device is not the device this configuration was made for, or the device has changed.\n") );
		    LOG_MSG( m_Logger, LogTypes::Error, sError );
		    return;
		}

		// Given the device, make the Hal Pins object
		switch ( pCfgItem->type )
		{
		    case HIDItemType::Button:	    obj = new EMCHIDButton( sPinPrefix, pCfgItem, pDeviceItem ); break;
		    case HIDItemType::Value:	    obj = new EMCHIDValue( sPinPrefix,pCfgItem, pDeviceItem ); break;
		    case HIDItemType::Hatswitch:    obj = new EMCHIDHatSwitch( sPinPrefix,pCfgItem, pDeviceItem ); break;
		    case HIDItemType::LED:	    obj = new EMCHIDLED( sPinPrefix, pCfgItem, pDeviceItem ); break;
		    case HIDItemType::LCD:	    break;
		    case HIDItemType::OutputValue:  obj = new EMCHIDOutput( sPinPrefix, pCfgItem, pDeviceItem ); break;
		}
	    }
        }
        if ( obj != NULL )
	    hid_objects.push_back( obj );
    }
}



EMCHIDDevice::~EMCHIDDevice(void)
{
    if ( m_pDevice != NULL )
	delete m_pDevice;
    if ( m_pHidCfg != NULL )
	delete m_pHidCfg;

    m_pDevice = NULL;
    m_pHidCfg = NULL;

    for ( int i = 0; i < hid_objects.count(); i++ )
	delete hid_objects[i];
    hid_objects.clear();
}

void EMCHIDDevice::stop()
{
    LOG_MSG( m_Logger, LogTypes::Debug, "Received request to stop" );
    m_bRun = false;
}

void EMCHIDDevice::run()
{
    if ( m_pDevice == NULL )
        return;

    m_bRun = true;
        
    if ( !m_pDevice->Open() )
    {
	LOG_MSG( m_Logger, LogTypes::Error, QString("%1: ERROR: Failed to open usb device\n").arg(m_sModuleName) );
	return;
    }

    if ( !m_pDevice->Claim() )
    {
	m_pDevice->Close();
	LOG_MSG( m_Logger, LogTypes::Error, QString("%1: ERROR: Failed to claim usb device\n").arg(m_sModuleName) );
	return;
    }

    // Find the largest report so we can allocate a buffer to receive it.
    int nLongestReport = 0;
    std::map<byte, HID_ReportDetails_t>::iterator it;
    for ( it = m_pDevice->ReportInfo().Reports.begin(); it != m_pDevice->ReportInfo().Reports.end(); it++ )
    {
	if ( it->second.InReportLength > nLongestReport )
	    nLongestReport = it->second.InReportLength;
    }

    // When there is more than one report, the report id is inserted before the packet.
    bool bMultipleReports = m_pDevice->ReportInfo().Reports.size() > 1;
    if ( bMultipleReports )
	nLongestReport++;

    // Make convenient collections
    QVector<EMCHIDLCD *> lcds;
    for ( QVector<EMCHIDItem *>::iterator it = hid_objects.begin(); it != hid_objects.end(); it++ )
	if ( (*it)->m_pCfgItem->type == HIDItemType::LCD )
	    lcds.push_back( dynamic_cast<EMCHIDLCD *>(*it) );

    QVector<EMCHIDItem *> outputs;
    for ( QVector<EMCHIDItem *>::iterator it = hid_objects.begin(); it != hid_objects.end(); it++ )
	if ( (*it)->m_pCfgItem->type == HIDItemType::LED || (*it)->m_pCfgItem->type == HIDItemType::OutputValue )
	    outputs.push_back( *it );

    // Initialise LCDs
    for ( QVector<EMCHIDLCD *>::iterator it = lcds.begin(); it != lcds.end(); it++ )
	(*it)->Initialise( m_pDevice );

    // Request initial reports
    for ( std::map<byte, HID_ReportDetails_t>::iterator it = m_pDevice->ReportInfo().Reports.begin(); it != m_pDevice->ReportInfo().Reports.end(); it++ )
    {
	HID_ReportDetails_t &details = it->second;
	if ( details.InReportLength > 0 )
	{
	    m_pDevice->RequestReport( details.ReportId, REPORT_ITEM_TYPE_In, (byte)(details.InReportLength + (bMultipleReports?1:0)) );
	}
    }

    // Start USB processing asynchronously
    m_pDevice->StartAsync();

    try
    {
        HIDParser parser;
        QVarLengthArray<byte> buf;
	buf.resize(nLongestReport);

	QMap<int,bool> OutputReportChange;
        for ( std::map<byte, HID_ReportDetails_t>::iterator it = m_pDevice->ReportInfo().Reports.begin(); it != m_pDevice->ReportInfo().Reports.end(); it++ )
	{
	    HID_ReportDetails_t &details = it->second;
	    OutputReportChange.insert( details.ReportId, false );
	}

	bool bFirst = true;

        while ( m_bRun )
        {
            const unsigned long LOOP_TIMEOUT_MS = 40;
	    msleep(LOOP_TIMEOUT_MS);
	    
	    int n;
	    do 
	    {
		// Read
		LOG_MSG( m_Logger, LogTypes::Debug, "Check for new messages" );
		n = m_pDevice->AsyncInterruptRead( buf.data(), buf.size() );
		if ( n > 0 )
		{
		    LOG_MSG( m_Logger, LogTypes::Debug, QString( "Processing message of %1 bytes").arg(n) );

		    byte *pData = buf.data();
		    byte nReportLen = (byte)n;
		    byte nReportId = 0;
		    if ( bMultipleReports )
		    {
			// multiple reports
			nReportId = pData[0];
			pData++;
			nReportLen--;
		    }

		    // Decode the packet.  Values are extracted directly into the ReportItems structure.
		    parser.DecodeReport( pData, nReportLen, m_pDevice->ReportInfo().ReportItems, nReportId, REPORT_ITEM_TYPE_In );

		    // Update the pins - they will update their own values from the ReportItems structure
		    for ( QVector<EMCHIDItem *>::iterator it = hid_objects.begin(); it != hid_objects.end(); it++ )
		    {
			(*it)->UpdatePin();
		    }
		}
	    } while ( m_bRun && n > 0 );

            // Check if we have usb outputs that need to be sent
	    for ( QMap<int,bool>::iterator it = OutputReportChange.begin(); it != OutputReportChange.end(); it++ )
		it.value() = false;

	    LOG_MSG( m_Logger, LogTypes::Debug, "Update outputs" );
            bool bChange = false;
            for ( QVector<EMCHIDItem *>::iterator it = hid_objects.begin(); it != hid_objects.end(); it++ )
	    {
		bool bItemChanged = (*it)->CheckOutputs();
		if ( bItemChanged )
		    OutputReportChange[(*it)->m_pDeviceItem->ReportID] = true;
		bChange = bChange || bItemChanged;
	    }
            
            if ( m_bRun && (bChange || bFirst) )
            {
                LOG_MSG( m_Logger, LogTypes::Debug, "There are changes to be sent" );
                // Should be a bit smarter here.  
                // Send all output reports
                for ( std::map<byte, HID_ReportDetails_t>::iterator it = m_pDevice->ReportInfo().Reports.begin(); it != m_pDevice->ReportInfo().Reports.end(); it++ )
                    if ( (bFirst || OutputReportChange[(it->second).ReportId]) &&  (it->second).OutReportLength > 0 )
                    {
                        byte nReportId = (it->second).ReportId;

                        // make a buffer
                        int nReportLen = bMultipleReports ? 1 : 0;
                        nReportLen += (it->second).OutReportLength;

			QVarLengthArray<byte> outBuf;
			outBuf.resize( nReportLen );
                        byte *ptr = outBuf.data();
                        int len = nReportLen;
                        if ( bMultipleReports )
                        {
	                    *ptr = nReportId;
	                    ptr++;
	                    len--;
                        }
                        // Make the report
                        parser.MakeOutputReport( ptr, (byte)len, m_pDevice->ReportInfo().ReportItems, nReportId );

                        // Send the report
                        LOG_MSG( m_Logger, LogTypes::Debug, QString( "Sending report %1").arg(nReportId) );
                        m_pDevice->AsyncInterruptWrite( outBuf.data(), nReportLen );
                    }
            }
            else
            {
                LOG_MSG( m_Logger, LogTypes::Debug, "No changes" );
            }

            // Check LCDs.  They have a timed refresh period, and do their own updating (they send a whole report, or 2, at a time)
	    if ( m_bRun )
	    {
		LOG_MSG( m_Logger, LogTypes::Debug, "Checking for LCD changes" );
		for ( QVector<EMCHIDLCD *>::iterator it = lcds.begin(); it != lcds.end(); it++ )
		    (*it)->Refresh( m_pDevice );
	    }

            bFirst = false;
        }
    }
    catch (...)
    {
    }

    LOG_MSG( m_Logger, LogTypes::Debug, "Thread terminating" );

    m_pDevice->StopAsync();
    m_pDevice->Unclaim();
    m_pDevice->Close();
}

