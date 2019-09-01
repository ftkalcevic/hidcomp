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


// hidcomp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "usages.h"

#include "log.h"
#include "logcore.h"
#include "hid.h"
#include "hiddevices.h"
#include "emchiddevice.h"
#include "emcinterface.h"
#include "emcglb.h"


EmcInterface *emcIFace;

#define EMC_DEBUG emc_debug

const int MAIN_THREAD_SLEEP_MS = 100;	// 100ms
const int EMC_POLL_TIME_MS = 50;	// 50ms
const unsigned int MODULE_NAME_LEN = HAL_NAME_LEN-20;	    // magic number from hal source code

#ifdef _WIN32
    #define signal(x,y)
    #define SIGINT 0
    #define SIGTERM 0
    int hal_init(const char * /*name*/) { return HAL_SUCCESS; }
    int hal_exit(int /*comp_id*/) { return HAL_SUCCESS; }
    void *hal_malloc(long int size) { return (void *)new char [size]; }
    int hal_pin_new(const char * /*name*/, hal_type_t /*type*/, hal_pin_dir_t /*dir*/, void **data_ptr_addr, int /*comp_id*/) { *data_ptr_addr = (void *)new char[20]; memset(*data_ptr_addr,0,20); return HAL_SUCCESS; }
    int hal_ready(int /*comp_id*/) { return 0; }
    int getopt_long(int /*argc*/, char * const /*argv*/[], const char * /*optstring*/, const struct option * /*longopts*/, int * /*longindex*/) { return -1; }
    char *optarg;
    int optind=1, opterr, optopt;
    struct option {  const char *name;  int has_arg;  int *flag;  int val;};
#else
    #include <signal.h>
    #include <getopt.h>
#endif

static bool bDone = false;
static bool bRunning = false;

static void quit(int /*sig*/) 
{
    bDone = true;
    if ( !bRunning )	// if we aren't running yet, we are still configuring, so terminate
	exit(0);
}


static struct option long_options[] = {
    {"name", 1, 0, 'n'},
    {"debug", 0, 0, 'd'},
    {"ini", 1, 0, 'i'},
    {"version", 0, 0, 'v'},
    {0,0,0,0}
};

static const char *option_string = "n:di:v";

int main(int argc, char* argv[])
{
    QCoreApplication app( argc, argv );
    app.setApplicationName( "hidcomp" );

    Logger m_Logger( app.applicationName(), "main" );

    LOG_MSG( m_Logger, LogTypes::Info, QString( "hidcomp %1.%2").arg(HIDCOMP_VERSION_MAJOR).arg(HIDCOMP_VERSION_MINOR) );

    int retval = 0;
    bDone = false;

    // loadusr hidcomp [-name module_name] -ini emc.ini config1.halhid [config2.halhid ...]
    QString sModuleName = "hidcomp";

    // parse the command line
    int opt;
    optind = 1;
    while ((opt=getopt_long_only(argc, argv, option_string, long_options, NULL)) != -1) 
    {
	switch(opt) 
	{
	    case 'n':   // component module name
		if (strlen(optarg) > MODULE_NAME_LEN) 
		{
		    LOG_MSG( m_Logger, LogTypes::Error, QString("hidcomp: ERROR: HAL module name too long: %1").arg(optarg) );
		    return -1;
		}
		sModuleName = optarg;
		LOG_MSG( m_Logger, LogTypes::Debug, QString("Read argument module name: '%1'").arg(sModuleName) );
		break;

	    case 'd':
		LogCore::SetLog( true );
		HIDDevices::Open( 255 );
		EMC_DEBUG=0xFFFFFFFF;
		LOG_MSG( m_Logger, LogTypes::Debug, QString("Read argument debug") );
		break;
		
	    case 'v':
    		LOG_MSG( m_Logger, LogTypes::Info, QString( "hidcomp %1.%2").arg(HIDCOMP_VERSION_MAJOR).arg(HIDCOMP_VERSION_MINOR) );
		break;

	    default:
		break;
	}
    }

    if ( optind >= argc )
    {
	// No config file
	LOG_MSG( m_Logger, LogTypes::Error, "hidcomp: Missing config file on command line" );
	return -1;
    }

    // All ready to go.
    signal(SIGINT, quit);
    signal(SIGTERM, quit);

    /* create HAL component */
    int hal_comp_id = hal_init( sModuleName.toLatin1().constData() );
    if ((hal_comp_id < 0) || bDone) 
    {
        LOG_MSG( m_Logger, LogTypes::Error, QString("%1: ERROR: hal_init failed: %2").arg(sModuleName).arg(hal_comp_id ));
        return hal_comp_id;
    }

    try
    {
        // read the config file(s)
        std::vector<EMCHIDDevice *> devices;
        int nInstance = 0;
        for ( int i = optind; i < argc; i++, nInstance++ )
        {
            // Load the configuration
            QString  sFilename = argv[i];
	    if ( sFilename.startsWith(QChar('-')) )
		break;

            QString sPinPrefix = QString("%1.%2").arg(sModuleName).arg(nInstance);

	    LOG_MSG( m_Logger, LogTypes::Debug, QString("Using hid config file '%1'").arg(sFilename) );
            devices.push_back( new EMCHIDDevice( sModuleName, sPinPrefix, sFilename ) );
        }

	if ( devices.size() == 0 )
	{
	    LOG_MSG( m_Logger, LogTypes::Error, QString("Failed to load any config files") );
	    retval = -1;
	    throw;
	}

        // count the total number of pins so we can grab some hal memory
        unsigned int nPins = 0;
        for ( unsigned int i = 0; i < devices.size(); i++ )
            for ( int j = 0; j < devices[i]->hid_objects.size(); j++ )
	        nPins += (unsigned int)devices[i]->hid_objects[j]->Pins.size();

	/* grab some shmem to store the HAL data in */
	void **haldata = (void **)hal_malloc( nPins * sizeof(void *) );
	if ((haldata == 0) || bDone) {
	    LOG_MSG( m_Logger, LogTypes::Error, QString("%1: ERROR: unable to allocate shared memory").arg(sModuleName));
	    retval = -1;
	    throw;
	}

	// Create the hal pins
	int n = 0;
        for ( unsigned int i = 0; i < devices.size(); i++ )
            for ( int j = 0; j < devices[i]->hid_objects.size(); j++ )
                for ( unsigned int k = 0; k < devices[i]->hid_objects[j]->Pins.size(); k++ )
	        {
		    EMCPin &pin = devices[i]->hid_objects[j]->Pins[k];
		    pin.pData = haldata + n;
		    int nRet = hal_pin_new( pin.sPinName.toLatin1().constData(), pin.nHalType, pin.nHalDir, (void **)pin.pData, hal_comp_id);			
		    if ( nRet != HAL_SUCCESS )
		    {
		        LOG_MSG( m_Logger, LogTypes::Error, QString("%1: ERROR: hal_pin_new failed for pin '%2': %3").arg(sModuleName).arg(pin.sPinName).arg(n) );
		        retval = n;
		        throw;
		    }
                    n++;
	        }

	// check if we have an lcd.  If so we need the emc interface.
	bool bHasLCD = false;
        for ( unsigned int i = 0; i < devices.size(); i++ )
            for ( int j = 0; j < devices[i]->hid_objects.size(); j++ )
		if ( devices[i]->hid_objects[j]->m_pCfgItem->type == HIDItemType::LCD )
		    bHasLCD = true;

	// ready
	hal_ready( hal_comp_id );

	if ( bHasLCD )
	{
	    emcIFace = new EmcInterface();
	    if ( !emcIFace->Initialise( argc, argv ) )
		throw;
	}

        // Each device does its own work in a background thread.  Just kick them off.
        for ( unsigned int i = 0; i < devices.size(); i++ )
            devices[i]->start();

	// Wait less for LCD as we need to check emc statuses
	int nWaitTime = MAIN_THREAD_SLEEP_MS;
	if ( bHasLCD )
	    nWaitTime = EMC_POLL_TIME_MS;

        // main loop
	bRunning = true;
        while ( !bDone )
        {
            // I can't find a sleep in QT, so we wait on the first thread till timeout.
            devices[0]->wait( nWaitTime );

            // if any of the threads die, abort.
            for ( unsigned int i = 0; i < devices.size(); i++ )
                if ( devices[i]->isFinished() )
                {
		    LOG_MSG( m_Logger, LogTypes::Error, QString("Device %1 failed unexpectedly.  Terminating.").arg(sModuleName) );
                    bDone = true;
                }

	    if ( bHasLCD && emcIFace != NULL )
		emcIFace->Update();
        }
	bRunning = false;
	LOG_MSG( m_Logger, LogTypes::Debug, "Terminating" );

        // Tell the threads to stop
        for ( unsigned int i = 0; i < devices.size(); i++ )
            devices[i]->stop();

        // Wait a bit for threads to terminate
        const unsigned long THEAD_WAIT_MSEC = 1000;
        for ( unsigned int i = 0; i < devices.size(); i++ )
	{
            devices[i]->wait( THEAD_WAIT_MSEC );
            devices[i]->terminate();
	}

	// Clean up the memory.
	for ( unsigned int i = 0; i < devices.size(); i++ )
	    delete devices[i];
	devices.clear();

	if ( emcIFace != NULL )
	{
	    emcIFace->Close();
	    delete emcIFace;
	    emcIFace = NULL;
	}

	LOG_MSG( m_Logger, LogTypes::Debug, "Clean up done" );
    }
    catch (...)
    {
    }

    hal_exit(hal_comp_id);

    return retval;
}

/*
reset device on exit(?)
check initial output values on startup
debug/error handling
test
    - build complete thingy
    - x/z
    - homing
    - zeroing
    - tool change notification
    - change to page #
*/
