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
#include "hidconfig.h"
#include "hid.h"
#include "hiddevices.h"
#include "selectdevice.h"
#include "hiduibutton.h"
#include "hiduivalue.h"
#include "hiduioutputvalue.h"
#include "hiduiled.h"
#include "hiduilcd.h"
#include "hiduihatswitch.h"
#include "hiddatathread.h"
#include "logcore.h"
#include "usages.h"

// Specials
//#define USAGE_PAGE_GENERIC_DESKTOP			1
//#define USAGE_PAGE_ALPHANUMERIC_DISPLAY			0x14
//#define	    USAGE_ALPHANUMERIC_DISPLAY			1
//#define	    USAGE_DISPLAY_ATTRIBUTES_REPORT		0x20
//#define	USAGE_HAT_SWITCH				0x39

#define PIN_NAME_LENGTH	40


hidconfig::hidconfig(QWidget *parent, Qt::WFlags flags) 
  : QMainWindow(parent, flags),
    m_pThread(NULL),
    m_device(NULL),
    m_pDeviceCriteria(NULL),
    m_pHidCfg( NULL ),
    m_Logger( QCoreApplication::applicationName(), "Browse" )
{
    ui.setupUi(this);
    m_MRU.setMenu( ui.menuFile );
    connect( &m_MRU, SIGNAL(MRUSelected(const QString &)), this, SLOT(onMRUSelected(const QString &)) );
    readSettings();
}

hidconfig::~hidconfig()
{

}

void hidconfig::writeSettings()
{
    m_Settings.setValue("window/size", size());
    m_Settings.setValue("window/pos", pos());

    for ( int i = 0; i < MAX_MRU; i++ )
	if ( i < m_MRU.count() )
	    m_Settings.setValue( QString("application/mru%1").arg(i), m_MRU[i] );
	else
	    m_Settings.setValue( QString("application/mru%1").arg(i), "" );

    m_Settings.setValue( "window/layout", this->saveState() );
}

void hidconfig::readSettings()
{
    resize(m_Settings.value("window/size", size()).toSize());
    move(m_Settings.value("window/pos", pos()).toPoint());

    for ( int i = MAX_MRU-1; i >= 0; i-- )
    {
	QString sMRU = m_Settings.value(QString("application/mru%1").arg(i), "" ).toString();
	if ( sMRU.length() > 0 )
	    m_MRU.append( sMRU );
    }

    if ( m_Settings.contains( "window/layout" ) )
	this->restoreState( m_Settings.value( "window/layout", QByteArray()).toByteArray() );
}

void hidconfig::onSelectionChangedHIDDevice()
{
}

void hidconfig::onAbout()
{
    QMessageBox msg(this);
    msg.setWindowTitle( "About HID Config" );
    msg.setTextFormat( Qt::RichText );
    msg.setText( QString("<h1><b>HIDConfig version %1.%2</b></h1>").arg(HIDCOMP_VERSION_MAJOR).arg(HIDCOMP_VERSION_MINOR) );
    msg.setInformativeText( "<p>Configuration program for hidcomp</p>"
			    "<p>Copyright (C) 2011 Frank Tkalcevic.</p>"
			    "<p/>"
			    "<p>This is free software, and you are welcome to redistribute it under certain conditions.  See the file COPYING, included.</p>"
			    "<p/>"
			    "<p>Visit my website to check for updates: <a href=\"http://www.franksworkshop.com.au/\">http://www.franksworkshop.com.au/</a></p>" 
			    "<p>For more information on EMC, visit the EMC web site: <a href=\"http://www.linuxcnc.org/\">http://www.linuxcnc.org/</a></p>" );
    msg.setIcon( QMessageBox::Information );
    msg.setIconPixmap( QPixmap(":/hidconfig/icon") );
    msg.exec();
    return;
}

void hidconfig::onExit()
{
    close();
}

void hidconfig::closeEvent( QCloseEvent * event )
{
    if ( !SaveChanges() )
    {
	event->ignore();
	return;
    }

    if ( m_pThread != NULL )
    {
	if ( m_pThread->isRunning() )
	{
	    m_pThread->stop();
	    m_pThread->wait( HIDDataThread::LOOP_TIMEOUT * 2 );
	}
    }
    writeSettings();
    event->accept();
}

void hidconfig::onOpenFile()
{
    QString sFile = QFileDialog::getOpenFileName( this, "Open HAL HID configuration", m_sLastFile, QString("HID config file (*.hid);;All files (*)") );
    if ( sFile.isNull() || sFile.isEmpty() )
	return;

    openFile( sFile );
}

void hidconfig::onMRUSelected(const QString &sFile)
{
    openFile( sFile );
}


void hidconfig::openFile( QString sFile )
{
    m_sLastFile = sFile;
    m_MRU.append( m_sLastFile );
    if ( m_pHidCfg != NULL )
	delete m_pHidCfg;

    // Parse the XML and create the HID configuration data.
    m_pHidCfg = HID::CreateFromXML( m_sLastFile );
    if ( m_pHidCfg == NULL )
    {
	QMessageBox::critical( this, "Can't read file", QString("Failed to read data file '%1'.  Check the stderr for errors").arg(m_sLastFile) );
	return;
    }

    // Search the USB devices for one that matches the criteria.
    HIDDevices hidDevices;
    std::vector<HIDDevice *> Devices = hidDevices.SearchHIDDevices( m_pHidCfg->criteria->bPID, m_pHidCfg->criteria->nPID, m_pHidCfg->criteria->bVID, m_pHidCfg->criteria->nVID, m_pHidCfg->criteria->bManufacturer, m_pHidCfg->criteria->sManufacturer, m_pHidCfg->criteria->bProduct, m_pHidCfg->criteria->sProduct, m_pHidCfg->criteria->bSerialNumber, m_pHidCfg->criteria->sSerialNumber, m_pHidCfg->criteria->bSystemId, m_pHidCfg->criteria->sSystemId, m_pHidCfg->criteria->bInterfaceNumber, m_pHidCfg->criteria->nInterfaceNumber );

    // If the number of USB devices we match is not exactly one, it is an error, and we can't procede.
    if ( Devices.size() != 1 )
    {
	QString sCriteria( "Criteria = '");
        if ( !m_pHidCfg->criteria->bPID && !m_pHidCfg->criteria->bVID && !m_pHidCfg->criteria->bManufacturer && !m_pHidCfg->criteria->bProduct && !m_pHidCfg->criteria->bSerialNumber && !m_pHidCfg->criteria->bSystemId && !m_pHidCfg->criteria->bInterfaceNumber  )
	    sCriteria = "Criteria = None Specified";
	else
	{
	    if ( m_pHidCfg->criteria->bPID )
		sCriteria += QString("PID: %1 ").arg(FormatPID(m_pHidCfg->criteria->nPID));
	    if ( m_pHidCfg->criteria->bVID )
		sCriteria += QString("VID: %1 ").arg(FormatVID(m_pHidCfg->criteria->nVID));
	    if ( m_pHidCfg->criteria->bManufacturer )
		sCriteria += QString("Manufacturer: %1 ").arg(m_pHidCfg->criteria->sManufacturer);
	    if ( m_pHidCfg->criteria->bProduct )
		sCriteria += QString("Product: %1 ").arg(m_pHidCfg->criteria->sProduct);
	    if ( m_pHidCfg->criteria->bSerialNumber )
		sCriteria += QString("SerialNumber: %1 ").arg(m_pHidCfg->criteria->sSerialNumber);
	    if ( m_pHidCfg->criteria->bSystemId )
		sCriteria += QString("SystemId: %1 ").arg(m_pHidCfg->criteria->sSystemId);
            if ( m_pHidCfg->criteria->bInterfaceNumber )
                sCriteria += QString("Interface No: %1 ").arg(m_pHidCfg->criteria->nInterfaceNumber);
            sCriteria += "'";
	}

	QMessageBox msg(this);
	msg.setWindowTitle( tr("Can't find device") );
	if ( Devices.size() == 0 )
	{
	    msg.setText( tr("Failed to find any USB HID devices matching the saved criteria.  Exit the application and try again.") );
	    msg.setDetailedText( sCriteria );
	}
	else
	{
	    msg.setText( tr("Matched %1 USB HID devices withe saved criteria.  Exit the application and try again.").arg(Devices.size()) );
	    QString sMatches;
	    for ( int i = 0; i < (int)Devices.size(); i++ )
	    {
                sMatches += QString( tr("PID:%1 VID:%2 Manufacturer:'%3' Product:'%4' Serial No.:'%5' System Id:'%6' Interface No:%7\n" ).arg(FormatPID(Devices[i]->PID())).arg(FormatVID(Devices[i]->VID())).arg(Devices[i]->Manufacturer()).arg(Devices[i]->Product()).arg(Devices[i]->SerialNumber()).arg(Devices[i]->SystemId()).arg(Devices[i]->InterfaceNumber()) );
	    }

	    msg.setDetailedText( tr("%1\nMatches:\n%2\n").arg(sCriteria).arg(sMatches) );
	}
	msg.setIcon( QMessageBox::Critical );
	msg.exec();
	return;
    }

    // Create the Qt display of this device.  It is populated with defaults.  We'll match usb items and set the values next.
    m_device = Devices[0];
    DisplayDevice();

    if ( m_HIDDisplayItems.size() == 0 )
    {
	QMessageBox msg(this);
	msg.setWindowTitle( tr("Can't read device") );
	msg.setText( tr("Unable to read the device details.  This is probably a permissions problem.") );
	msg.setIcon( QMessageBox::Critical );
	msg.exec();
	return;
    }

    // Once a device si loaded, we can't open another, or create a new one (just lazy programming - no clean up code)
    SetLoaded();

    // set the criteria
    m_pDeviceCriteria->setCriteria( m_pHidCfg->criteria );

    // now go through the configration items and match them up to the actual HID device items (visual items)
    for ( unsigned int i = 0; i < m_pHidCfg->items.size(); i++ )
    {
	HIDItem *pItem = m_pHidCfg->items[i];
	HIDUIBase *pUIItem = (HIDUIBase *)m_HIDDisplayItems[i];

	QString sMismatch;
	if ( pUIItem->type() != pItem->type )
	    sMismatch += QString("Incorrect type.  Expected '%1' got '%2'\n").arg(pUIItem->type()).arg(pItem->type);
        if ( pItem->type != HIDItemType::LCD )
        {
	    if ( pUIItem->ReportItem()->Attributes.UsagePage != pItem->nUsagePage )
	        sMismatch += QString("Incorrect usage page.  Expected '%1' got '%2'\n").arg(pUIItem->ReportItem()->Attributes.UsagePage).arg(pItem->nUsagePage);
	    if ( pUIItem->ReportItem()->Attributes.Usage != pItem->nUsage )
	        sMismatch += QString("Incorrect usage.  Expected '%1' got '%2'\n").arg(pUIItem->ReportItem()->Attributes.Usage).arg(pItem->nUsage);
        }
	if ( !sMismatch.isEmpty() )
	{
	    QMessageBox msg( this );
	    msg.setWindowTitle( tr("Item mismatch") );
	    msg.setText( tr("There was a problem matching up item index %1 when reloading the configuration.").arg(pItem->nIndex) );
	    msg.setInformativeText( tr("This means the device is not the device this configuration was made for, or the device has changed.") );
	    msg.setDetailedText( sMismatch );
	    msg.setIcon( QMessageBox::Critical );
	    msg.exec();
	    return;
	}

	pUIItem->setConfig( pItem );
    }
    updateWindowTitle();
}

void hidconfig::onSaveFile()
{
    DoSave();
}

bool hidconfig::DoSave()
{
    if ( m_pHidCfg == NULL )
	delete m_pHidCfg;

    m_pHidCfg = new HID;
    m_pHidCfg->criteria = new HIDDeviceCriteria;

    m_pDeviceCriteria->getCriteria( m_pHidCfg->criteria );

    if ( !m_pHidCfg->criteria->bPID && !m_pHidCfg->criteria->bVID && !m_pHidCfg->criteria->bManufacturer && !m_pHidCfg->criteria->bProduct && !m_pHidCfg->criteria->bSerialNumber && !m_pHidCfg->criteria->bSystemId && !m_pHidCfg->criteria->bInterfaceNumber )
    {
	QMessageBox::warning(this, "No criteria", "No criteria has been set.  You may not be able use the configuration if it doesn't uniquely identify the device.  Use the 'Test Criteria' button to test it." );
    }
    else
    {
	HIDDevices hidDevices;
        std::vector<HIDDevice *> matches = hidDevices.SearchHIDDevices( m_pHidCfg->criteria->bPID, m_pHidCfg->criteria->nPID, m_pHidCfg->criteria->bVID, m_pHidCfg->criteria->nVID, m_pHidCfg->criteria->bManufacturer, m_pHidCfg->criteria->sManufacturer, m_pHidCfg->criteria->bProduct, m_pHidCfg->criteria->sProduct, m_pHidCfg->criteria->bSerialNumber, m_pHidCfg->criteria->sSerialNumber, m_pHidCfg->criteria->bSystemId, m_pHidCfg->criteria->sSystemId, m_pHidCfg->criteria->bInterfaceNumber, m_pHidCfg->criteria->nInterfaceNumber );
	if ( matches.size() != 1 )
	    if ( QMessageBox::warning( this, "Bad Criteria", QString("The criteria specified matches %1 HID devices.  If you continue to save the configuration, you will probably not be able to reload this file, or use it with HIDCOMP until the criteria matches exactly 1 device.").arg(matches.size()), QMessageBox::Ok | QMessageBox::Cancel ) == QMessageBox::Cancel )
		return false;
    }

    // Check for pins for duplicates or name too long
    QString sPrefix = "hidcomp.0";
    QString sInformativeText;
    QList<QString> pins; 
    for ( int i = 0; i < (int)m_HIDDisplayItems.size(); i++ )
    {
	HIDUIBase *pUIItem = m_HIDDisplayItems[i];
	if ( pUIItem->enabled() )
	{
	    QList<QString> p = pUIItem->PinNames( sPrefix );

	    for ( int j = 0; j < p.count(); j++ )
	    {
		if ( p[j].length() > PIN_NAME_LENGTH )
		    sInformativeText += QString("Pin name too long: '%1': maximum length=%2, actual length=%3\n").arg(p[j]).arg(PIN_NAME_LENGTH).arg(p[j].length());
		if ( pins.contains(p[j]) )
		    sInformativeText += QString("Duplicate pin name: '%1'\n").arg(p[j]);
		else
		    pins.push_back( p[j] );
	    }
	}
    }
    if ( !sInformativeText.isEmpty() )
    {
	QMessageBox msg( QMessageBox::Critical, "Pin Error", "There was a problem with the hal pin definitions.  See the detailed text for more informaton.", QMessageBox::NoButton, this, Qt::Dialog );
	msg.setDetailedText( sInformativeText );
	msg.exec();
	return false;
    }

    // Build output
    for ( int i = 0; i < (int)m_HIDDisplayItems.size(); i++ )
    {
	HIDUIBase *pUIItem = m_HIDDisplayItems[i];
	HID_ReportItem_t *pUSBItem = pUIItem->ReportItem();
        HIDItem *pCfgItem = NULL;
        if ( pUSBItem == NULL )
	    pCfgItem = HIDItem::CreateItem( pUIItem->type(), pUIItem->Index(), 0, 0, pUIItem->enabled(), pUIItem->name() );
        else
	    pCfgItem = HIDItem::CreateItem( pUIItem->type(), pUIItem->Index(), pUSBItem->Attributes.UsagePage, pUSBItem->Attributes.Usage, pUIItem->enabled(), pUIItem->name() );

	pUIItem->getConfig( pCfgItem );
	m_pHidCfg->items.push_back( pCfgItem );
    }

    for (;;)
    {
	QString sFilename = QFileDialog::getSaveFileName( this, "Save HID configuration", m_sLastFile, QString("HID config file (*.hid);;All files (*)") );
	if ( sFilename.isEmpty() || sFilename.isNull() )
	    return false;

	QFileInfo fi(sFilename);
	if ( fi.suffix().isEmpty() )
	    sFilename.append( ".hid" );

	QDomDocument doc = m_pHidCfg->MakeXML();

	QFile file(sFilename.toLatin1().constData());
	if ( !file.open( QIODevice::WriteOnly | QIODevice::Truncate ) )
	{
	    QMessageBox::critical( this, "Can't save", QString("Failed to save file '%1':%2").arg(sFilename).arg(file.errorString()) );
	}
	else
	{
	    m_sLastFile = sFilename;
	    m_MRU.append( m_sLastFile );

	    file.write( doc.toString().toLatin1() );
	    file.close();
	    break;
	}
    }
    updateWindowTitle();
    return true;
}


void hidconfig::onNew()
{
    SelectDevice dlg( m_hidDevices, this );
    if ( dlg.exec() == QDialog::Accepted )
    {
	m_device = dlg.m_pSelection;

	DisplayDevice();
	SetLoaded();
    }
}

// once loaded, prevent another config from being created or
// loaded from file. (Just lazy programming that doesn't clean up properly)
void hidconfig::SetLoaded()
{
    ui.actionNew->setEnabled( false );
    ui.actionOpen->setEnabled( false );
    m_MRU.setEnabled( false );
}

void hidconfig::onDebug()
{
    bool debug = ui.actionDebug->isChecked();
    if ( debug )
        HIDDevices::Open( 255 );
    else
        HIDDevices::Open( 0 );

    LogCore::SetLog(debug);
}

void hidconfig::DisplayDevice()
{
    setCentralWidget( ui.itemArea );

    QGroupBox *outerbox = new QGroupBox();
    ui.itemArea->setWidget( outerbox );

    QVBoxLayout *layout = new QVBoxLayout();
    outerbox->setLayout( layout);

    m_pDeviceCriteria = new DeviceWidget( NULL, m_device );
    layout->addWidget( m_pDeviceCriteria );

    QGroupBox *box = new QGroupBox(tr("HID Device Input/Output items"));
    layout->addWidget( box );

    QGridLayout *item_layout = new QGridLayout();
    box->setLayout( item_layout );

    m_device->PreprocessReportData();
    for ( int i = 0; i < (int)m_device->ReportInfo().ReportItems.size(); i++ )
    {
	HID_ReportItem_t *item = m_device->ReportInfo().ReportItems[i];

	if ( (item->ItemFlags & IOF_CONSTANT) == 0 )
	{
	    HIDUIBase *pItem = NULL;
	    if ( item->ItemType == REPORT_ITEM_TYPE_In )
	    {
		// Define a button as any input with a bit size of 1.
		if ( item->BitSize == 1 )
		    pItem = new HIDUIButton(m_device, item, i, item_layout, item->Value );
		else if ( item->Attributes.UsagePage == USAGEPAGE_GENERIC_DESKTOP_CONTROLS && item->Attributes.Usage == USAGE_HATSWITCH )
		    pItem = new HIDUIHatswitch(m_device, item, i, item_layout, item->Value);
		else
		{
		    HIDUIValue *pValueItem = new HIDUIValue(m_device, item, i, item_layout, item->Value);
		    pItem = pValueItem;
		    layout->addWidget( pValueItem );
		}
	    }
	    else if ( item->ItemType == REPORT_ITEM_TYPE_Out )
	    {
		if ( item->Attributes.UsagePage == USAGEPAGE_ALPHANUMERIC_DISPLAY )
		{
                    continue;       // ignore displays - we need a collection
		}
		else
		{
		    // Define an LED as any output with a bit size of 1.
		    if ( item->BitSize == 1 )
		    {
			HIDUILED *pLEDItem = new HIDUILED(m_device, item, i, item_layout, item->Value );
			connect( pLEDItem, SIGNAL(change( HID_ReportItem_t *)), this, SLOT(outputChanged( HID_ReportItem_t *)) );
			pItem = pLEDItem;
		    }
		    else
		    {
			HIDUIOutputValue *pValueItem = new HIDUIOutputValue(m_device, item, i, item_layout, item->Value);
			connect( pValueItem, SIGNAL(change( HID_ReportItem_t *)), this, SLOT(outputChanged( HID_ReportItem_t *)) );
			pItem = pValueItem;
		    }
		}
	    }

	    if ( pItem != NULL )
		m_HIDDisplayItems.push_back( pItem );
	}
    }

    // Go through collections and find USAGE_PAGE_ALPHANUMERIC_DISPLAY devices
    for ( int i = 0; i < (int)m_device->ReportInfo().Collections.size(); i++ )
    {
	HID_CollectionPath_t *col = m_device->ReportInfo().Collections[i];
	if ( col->UsagePage == USAGEPAGE_ALPHANUMERIC_DISPLAY && col->Usage == USAGE_ALPHANUMERIC_DISPLAY)
	{
	    HIDUIBase *pItem = new HIDUILCD(m_device, col, i, item_layout );
	    if ( pItem != NULL )
	    	m_HIDDisplayItems.push_back( pItem );
	}
    }

    m_pThread = new HIDDataThread( m_device );

    qRegisterMetaType<QVector<byte> >("QVector<byte>");
    connect(m_pThread, SIGNAL(newData( QVector<byte>  )), this, SLOT(onNewData( QVector<byte>  )));


    m_pThread->start();
}

// New data packet from the device
void hidconfig::onNewData( QVector<byte> data )
{
    LOG_MSG( m_Logger, LogTypes::Debug, QString("New data %1 bytes").arg(data.size()) );
    if ( m_Logger.WillLog( LogTypes::Debug ) )
    {
        QString sData;
        for ( int i = 0; i < data.size(); i++ )
            sData += QString("%1 ").arg(data[i],2,16,QChar('0'));
        LOG_MSG(m_Logger, LogTypes::Debug, sData );
    }

    // unpack the data
    byte nReportId = 0;
    if ( m_device->ReportInfo().Reports.size() > 1 )
    {
	// multiple reports
	nReportId = data[0];
	data.pop_front();
    }

    // Decode the packet
    HIDParser parser;
    parser.DecodeReport( data.data(), (byte)data.size(), m_device->ReportInfo().ReportItems, nReportId, REPORT_ITEM_TYPE_In );

    // Update the display
    std::vector<HIDUIBase *>::iterator it;
    for ( it = m_HIDDisplayItems.begin(); it != m_HIDDisplayItems.end(); it++ )
	(*it)->Update();
}


// one of the output items has changed (output from PC to device).
void hidconfig::outputChanged( HID_ReportItem_t *pItem )
{
    assert( pItem->ItemType == REPORT_ITEM_TYPE_Out );

    // an output item has changed.  Send the report it belongs to.
    byte nReportId = pItem->ReportID;

    // make a buffer
    int nBufferLen = m_device->ReportInfo().Reports.size() > 1 ? 1 : 0;
    nBufferLen += m_device->ReportInfo().Reports[nReportId].OutReportLength;

    unsigned char *buffer = new unsigned char [ nBufferLen ];
    unsigned char *ptr = buffer;
    int len = nBufferLen;
    if ( m_device->ReportInfo().Reports.size() > 1 )
    {
	*ptr = nReportId;
	ptr++;
	len--;
    }

    // Make the report
    HIDParser parser;
    parser.MakeOutputReport( ptr, (byte)len, m_device->ReportInfo().ReportItems, nReportId );
    if ( m_Logger.WillLog( LogTypes::Debug ) )
    {
        QString sDebug = QString("Sending %1 bytes ").arg(nBufferLen);
	for ( int i = 0; i < nBufferLen; i++ )
	    sDebug += QString(" %1").arg(buffer[i], 2, 16, QChar('0'));
        LOG_MSG( m_Logger, LogTypes::Debug, sDebug.toLatin1().constData() );
    }

    // Send the report
    int nRet = m_device->InterruptWrite( buffer, nBufferLen, USB_TIMEOUT );
    LOG_MSG( m_Logger, LogTypes::Debug, QString("interrupt write returned %1\n").arg(nRet).toLatin1().constData() );

    delete buffer;
}


void hidconfig::ProcessCommandline()
{
    QStringList args = QCoreApplication::instance()->arguments();

    if ( args.count() > 1 )
    {
        QString sFile = args[1];
        QFileInfo info( sFile );
        if ( !info.exists() )
            QMessageBox::critical( this, "File not found", QString("File '%1' does not exist").arg(sFile) );
        else
            openFile( sFile );
    }
}

void hidconfig::updateWindowTitle()
{
    QString s = QCoreApplication::applicationName();
    if ( !m_sLastFile.isEmpty() )
    {
	s += " - ";
	s += m_sLastFile;
    }
    setWindowTitle( s );
}


bool hidconfig::SaveChanges()
{
    if ( m_HIDDisplayItems.size() == 0 )    // nothing loaded
	return true;

    // Build output
    HID *pHidCfg = new HID;
    pHidCfg->criteria = new HIDDeviceCriteria;
    m_pDeviceCriteria->getCriteria( pHidCfg->criteria );
    for ( int i = 0; i < (int)m_HIDDisplayItems.size(); i++ )
    {
	HIDUIBase *pUIItem = m_HIDDisplayItems[i];
	HID_ReportItem_t *pUSBItem = pUIItem->ReportItem();
        HIDItem *pCfgItem = NULL;
        if ( pUSBItem == NULL )
	    pCfgItem = HIDItem::CreateItem( pUIItem->type(), pUIItem->Index(), 0, 0, pUIItem->enabled(), pUIItem->name() );
        else
	    pCfgItem = HIDItem::CreateItem( pUIItem->type(), pUIItem->Index(), pUSBItem->Attributes.UsagePage, pUSBItem->Attributes.Usage, pUIItem->enabled(), pUIItem->name() );

	pUIItem->getConfig( pCfgItem );
	pHidCfg->items.push_back( pCfgItem );
    }

    QString sCurrent = pHidCfg->MakeXML().toString();
    QString sOld;

    if ( m_pHidCfg != NULL )
	sOld = m_pHidCfg->MakeXML().toString();

    bool bChanges = (sCurrent != sOld);

    if ( !bChanges )
	return true;

    int nRet = QMessageBox::warning( this, "Save changes", "Save Changes", QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel );
    if ( nRet == QMessageBox::No )
	return true;
    else if ( nRet == QMessageBox::Cancel )
	return false;
    else if ( DoSave() )
        return true;
    else
        return false;
}



/*
TODO
- default button bad on lcd config dialog
- test more
- timer/watchdog, is there anything to do
*/


