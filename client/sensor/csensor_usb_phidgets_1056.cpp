/*
 *  csensor_usb_phidgets_1056.cpp
 *  qcn
 *
 *  Created by Carl Christensen on 08/11/2007.
 *  Copyright 2012 Stanford University.  All rights reserved.
 *
 * Implementation file for cross-platform (Mac, Windows, Linux) Phidgets 1056 accelerometer USB sensor class
 *     http://www.phidgets.com/products.php?product_id=1056
 *   NB: some "Windows" terminology used, i.e. m_WinDLLHandle but it's really a shared object Mac dylib or Linux so of course
 */

#include "main.h"
#include "csensor_usb_phidgets_1056.h"

//set the dll/so/dylib name
#ifdef _WIN32
   const char CSensorUSBPhidgets1056::m_cstrDLL[] = {"MotionNodeAccelAPI.dll"};
#else
#ifdef __APPLE_CC__
   const char CSensorUSBPhidgets1056::m_cstrDLL[] = {"libMotionNodeAccelAPI.dylib"};   
#else
   const char CSensorUSBPhidgets1056::m_cstrDLL[] = {"libMotionNodeAccelAPI.so"};   
#endif // apple or linux
#endif // windows˚˚
CSensorUSBPhidgets1056::CSensorUSBPhidgets1056()
  : CSensor(), 
     m_WinDLLHandle(NULL), m_SymHandle(NULL), m_node(NULL)
{ 
}

CSensorUSBPhidgets1056::~CSensorUSBPhidgets1056()
{
   closePort();
}

void CSensorUSBPhidgets1056::closePort()
{
    if (getPort() > -1) {
        fprintf(stdout, "Closing %s sensor port...\n", getTypeStr());
    }
    if (m_node) {
        if (m_node->is_connected() && m_node->is_reading()) {
           m_node->stop();  // if started & reading
        }
        m_node->close();
        delete m_node;
        m_node = NULL;
        setPort();
        setType();
        if (getPort() > -1) {
           fprintf(stdout, "Port closed!\n");
           fflush(stdout);
        }
    }

    // close MN dll
    if (m_WinDLLHandle) {
#ifdef __USE_DLOPEN__
        if (dlclose(m_WinDLLHandle)) {
           fprintf(stderr, "%s: dlclose error %s\n", getTypeStr(), dlerror());
        }
#else // probably Windows - free library
   #ifdef _WIN32
        ::FreeLibrary(m_WinDLLHandle);
   #endif
#endif
	m_WinDLLHandle = NULL;
    }
}

bool CSensorUSBPhidgets1056::detect()
{
   setType();
   setPort();

   if (qcn_main::g_iStop) return false;

   // always check dll existence & try to load!
   std::string sstrDLL;
#ifdef QCNLIVE  // it will already be in init, so just load the module name
   sstrDLL = "";
#else  // live boinc - check in project directory
   sstrDLL = (const char*) sm->dataBOINC.project_dir; 
   sstrDLL += qcn_util::cPathSeparator();
#endif
   sstrDLL += m_cstrDLL;
  
   if (!boinc_file_exists(sstrDLL.c_str())) {
       //fprintf(stderr, "MotionNode Accel dylib file %s not found\n", sstrDLL.c_str());
       return false;  // dylib not found, so return
   }

#ifdef __USE_DLOPEN__
   if (qcn_main::g_iStop) return false;

   m_WinDLLHandle = dlopen(sstrDLL.c_str(), RTLD_LAZY | RTLD_GLOBAL); // default
   if (!m_WinDLLHandle) {
       fprintf(stderr, "CSensorUSBPhidgets1056: dynamic library %s dlopen error %s\n", sstrDLL.c_str(), dlerror());
       return false;
   }

   if (qcn_main::g_iStop) return false;

   m_SymHandle = (PtrMotionNodeAccelFactory) dlsym(m_WinDLLHandle, "MotionNodeAccel_Factory");
   if (!m_SymHandle) {
       fprintf(stderr, "CSensorUSBPhidgets1056: Could not get dlsym MotionNode Accel dylib file %s - error %s\n", sstrDLL.c_str(), dlerror());
       return false;
   }

   m_node = (*m_SymHandle)(MOTIONNODE_ACCEL_API_VERSION);
#else // for Windows or not using dlopen just use the direct motionnode factory
   m_node = MotionNodeAccel::Factory();
#endif

   if (!m_node) {
      fprintf(stderr, "CSensorUSBPhidgets1056: Could not make MotionNode Factory\n");
      return false; // not found
   }

   if (qcn_main::g_iStop) return false;

   // Detect the number of available devices.
   unsigned int count = 0;
   m_node->get_num_device(count);
   if (!count) {
       closePort();
       return false;
   }

   if (qcn_main::g_iStop) return false;

   // Set the G range. Default is 2.
   if (!m_node->set_gselect(2.0)) {
       fprintf(stderr, "CSensorUSBPhidgets1056: Could not set range on MotionNode Accel\n");
       closePort();
       return false;
   }

   // set the sample rate to 100Hz to get at least 1 & possibly 2 samples at 50Hz
   /* from Luke Tokheim:
       Use the "set_delay" method before you connect, just like choosing the G 
    range. The delay ranges from 0 to 1. The available sample rates are from 
    50 to 100 Hz by 10 Hz increments. Compute the delay value with the 
    following formula:

    delay = 1 - (target_rate - minimum_rate) * 0.0125;

    So, to sample at 100 Hz set the delay to:

    1 - (100 - 50) * 0.0125 = 0.375

    To sample at 50 Hz, set the delay to:

    1 - (50 - 50) * 0.0125 = 1
   */
   if (!m_node->set_delay(0.0f)) {
       fprintf(stderr, "CSensorUSBPhidgets1056: Could not set delay time on MotionNode Accel\n");
       closePort();
       return false;
   }
   
   if (qcn_main::g_iStop) return false;

   if (!m_node->connect()) { // connect to the sensor
       fprintf(stderr, "CSensorUSBPhidgets1056: Could not connect to MotionNode Accel\n");
       closePort();
       return false;
   }

   if (qcn_main::g_iStop) return false;

   if (!m_node->start()) {
       fprintf(stderr, "CSensorUSBPhidgets1056: Could not start MotionNode Accel\n");
       closePort();
       return false;
   }

   // OK, at this point we should be connected, so from here on out can just read_xyz until closePort()
   // set as a single sample per point
   setSingleSampleDT(true);  // mn samples itself

   // NB: closePort resets the type & port, so have to set again 
   setType(SENSOR_USB_MOTIONNODEACCEL);
   setPort(getTypeEnum());

   return true;
}

inline bool CSensorUSBPhidgets1056::read_xyz(float& x1, float& y1, float& z1)
{
    //MotionNodeAccel::raw_type a[3];
    MotionNodeAccel::real_type a[3];  // for data calibrated to "g"
    a[0] = a[1] = a[2] = 0.0f;
    bool bRet = false;
    // note that x/y/z should be scaled to +/- 2g, return values as +/- 2.0f*EARTH_G (in define.h: 9.78033 m/s^2)
    // MotionNode returns +/-2g values so just multiply by EARTH_G

    if (m_node && m_node->sample() && m_node->get_sensor(a)) {
/*
  Error in motion node directions:
The attached plot shows how data is currently being displayed in QCNLive from the MotionNode on both Mac and Windows. This suggests that all of the components are mixed up: e.g. current Z should be X (north), current X should be Y (east), current Y should be Z (Vertical). 
//        x1 = a[0] * EARTH_G;
//        y1 = a[1] * EARTH_G;
//        z1 = a[2] * EARTH_G;
*/
#ifdef QCN_RAW_DATA	
        x1 = a[2];
        y1 = a[0];
        z1 = a[1];
#else
        x1 = a[2] * EARTH_G;
        y1 = a[0] * EARTH_G;
        z1 = a[1] * EARTH_G;
#endif
        bRet = true;
    }
    return bRet;
}
