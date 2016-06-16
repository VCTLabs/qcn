#ifndef _DEFINE_H_
#define _DEFINE_H_

/**
  \file    define.h
  \author  Carl Christensen, carlgt1@hotmail.com
  
 *  global defines used for the Quake Catcher Network client, graphics, and QCNLive programs

 *  Created by Carl Christensen on 08/11/2007.
 *
*/

#include <stdlib.h>
#include <vector>
#include <string>

#ifdef _WIN32
   #include "qcn_config_win.h"
   #include <direct.h>
#else
   #include <sys/select.h>
   #include "config.h"
#endif

using std::string;
using std::vector;


// define this to allow random uploading of sensor SAC files
// #define RANDOM_USB_UPLOAD

#ifndef _chdir
#define _chdir chdir
#endif

// ZipFileList is defined in boinc_zip
#ifdef ZIPARCHIVE
typedef vector<string> ZipFileList;
#endif

// first define some useful math functions
/*! 
   \def QCN_SQR(A} 
   takes \a A to the second power (i.e. \a A^2 or \a A**2) 
*/
#define QCN_SQR(A)   ((A)*(A))

/*! 
   \def QCN_ROUND(A)
   rounds \a A up or down from double/float to a long 
*/
#define QCN_ROUND(A) (((A)>=0) ? (long)((A) + .5) : (long)((A) - .5))

/*! 
   \def QCN_INPUT_LOGICAL_NAME "qcn_0"
   default BOINC/QCN input file logical name while will get resolved to a real input file in boinc/projects/qcn directory
*/
#define QCN_INPUT_LOGICAL_NAME "qcn_0"

// constants for the time sync - bumped up to half-hour between successful tries, 3 minutes for unsuccessful
#define FLOAT_TIME_SYNC_INTERVAL  1800.0f
#define FLOAT_TIME_SYNC_ERROR      180.0f


#ifdef _WIN32
  #include <windows.h>

// make a usleep function for Windows Sleep (which is in milliseconds)
  #define usleep(A) ::Sleep((DWORD) ((A)/1000L))
  #pragma warning( disable : 4786 )  // Disable warning messages for vector
  #pragma warning( disable : 4996 )  // Disable warning messages for "obsolete" libs

  #define CONSOLE_TITLE_FORMAT "qcn_PID#%012d"
#endif

#ifdef GRAPHICS_PROGRAM


#ifdef QCNLIVE
  #  define NUM_FONT          3
#else
  #  define NUM_FONT          2
#endif
#include "boinc_gl.h"  // note this goes before qgl to avoid duplicate refs for Gldouble etc

//#include "texfont.h"
#  define TTF_ARIAL         0
#  define TTF_MONOSPACE     1
#  define TTF_ARIAL_HUGE    2
#endif

#define KEY_SHIFT 16
#define KEY_CTRL  17

#ifndef GLUT_LEFT_BUTTON
// put glut keys here
// GLUT key definitions, although we're not actually using GLUT, just these key mappings!
#define  GLUT_NO_BUTTON                     -1
#define  GLUT_LEFT_BUTTON                   0x0000
#define  GLUT_MIDDLE_BUTTON                 0x0001
#define  GLUT_RIGHT_BUTTON                  0x0002
#define  GLUT_DOWN                          0x0000
#define  GLUT_UP                            0x0001
#define  GLUT_LEFT                          0x0000
#define  GLUT_ENTERED                       0x0001
#endif

// NULL values for SAC files
#define SAC_NULL_FLOAT  -12345.0f
#define SAC_NULL_LONG   -12345L

// JoyWarrior identification string, number of buttons, and number of axes
#define IDSTR_JW24F8       "Code Mercenaries JoyWarrior24 Force 8"
#define NUM_BUTTON_JW24F8  8
#define NUM_AXES_JW24F8    3

#define IDSTR_JW24F14       "Code Mercenaries JoyWarrior24F14"
#define NUM_BUTTON_JW24F14  8
#define NUM_AXES_JW24F14    3

// if the JW24F14 exceeds this amount in calibration then sensor is off 2g range
#define Z_CORRECTION_CUTOFF_JW24F14  15.0

// changed to one week per workunit
// #define WORKUNIT_COMPLETION_TIME_ELAPSED 604800.0f
// need to regenerate workunits to allow 24 * 7 upload slots
#define WORKUNIT_COMPLETION_TIME_ELAPSED 86400.0f
#define SIZEOF_PROJECT_PREFERENCES 131072

// the size of a self-appointed station string
#define SIZEOF_STATION_STRING 8
 
#if !defined(_WIN32) && !defined(__APPLE_CC__)
   // define Linux joystick device for JoyWarrior
   #define LINUX_JOYSTICK_NUM   9
   #define LINUX_JOYSTICK_ARRAY { \
          "/dev/js0", \
          "/dev/input/js0", \
          "/dev/input/event0", \
          "/dev/js1", \
          "/dev/input/js1", \
          "/dev/input/event1", \
          "/dev/js2", \
          "/dev/input/js2", \
          "/dev/input/event2", \
         }
#endif

// for the sensor try/catch
#define EXCEPTION_SHUTDOWN 1

//#define MAX_NUM_PHIDGETS 5
#define MAX_PERTURB 10

// earth's gravity in meters per second-squared
#define EARTH_G 9.78033f

#define DEFAULT_SIG_CUTOFF          3.0f
#define DEFAULT_SHORT_TERM_AVG_MAG 11.0f
#define MIN_RETRIGGER_SECONDS       3.0f     // min time allowed between triggers
#define MAX_TRIGGER_COUNT_MINUTE    1.0f     // max trigger count in a minute we can assume their sensor sucks or triggers too much, i.e. 2 per minute avg over a long period is a bit much

#define XML_SIG_CUTOFF         "<fsig>"
#define XML_SHORT_TERM_AVG_MAG "<fsta>"

enum e_perturb {
    PERTURB_SIG_CUTOFF,
    PERTURB_SHORT_TERM_AVG_MAG,
    PERTURB_3,
    PERTURB_4,
    PERTURB_5,
    PERTURB_6,
    PERTURB_7,
    PERTURB_8,
    PERTURB_9,
    PERTURB_10
};

// process ID offsets into the qcn_shmem_usb array (sm->alPID)
#define PID_USB  0
#define PID_QCN  1

// pipe identifiers for the fdPipe file descriptor array
#define PIPE_STATE  0
#define PIPE_SENSOR 1

#define ALL_ACCESS 0666

// trickle-down check & ping trickle interval is a half-hour
#define INTERVAL_PING_SECONDS 1800.0f

const float g_cfTimeWindow = 60.0f;  // time window in seconds

/*
// android has trouble keeping up?
#ifdef ANDROID
   const double g_DT = 0.05;          // delta t sampling interval, i.e. target time between points
   const double g_DT_SLOW = 0.20;      // delta t sampling interval for slow/troublesome machines (i.e. can't keep up at <3 samples per dt=.02)
#else
*/
   const double g_DT = 0.02;          // delta t sampling interval, i.e. target time between points
   const double g_DT_SLOW = 0.10;      // delta t sampling interval for slow/troublesome machines (i.e. can't keep up at <3 samples per dt=.02)
//#endif
const double g_DT_SNAIL = 0.20;      // for horrible machines but maybe let them run for educational purposes?

// common QCN defines & return codes can go here

enum e_os      { OS_MAC_INTEL, OS_MAC_PPC, OS_WINDOWS, OS_LINUX };

enum e_retcode { ERR_NONE = 0, ERR_FINISHED = 1, ERR_NO_SENSOR = 99, ERR_SHMEM, ERR_TIME_CHANGE, ERR_INIT, 
    ERR_CRASH, ERR_DIR_TRIGGER, ERR_DIR_IMAGES, ERR_INPUT_FILE, ERR_INPUT_PARSE, 
    ERR_ABORT, ERR_NUM_RESET, ERR_HEARTBEAT, ERR_SUSPEND, ERR_CREATE_THREAD, ERR_SIGNAL };

// values in the qcn_variety table
enum e_trigvariety { TRIGGER_VARIETY_FINALSTATS = -2, TRIGGER_VARIETY_QUAKELIST, TRIGGER_VARIETY_NORMAL, TRIGGER_VARIETY_PING, TRIGGER_VARIETY_CONTINUAL };

enum e_maxmin  { E_DX, E_DY, E_DZ, E_DS };

enum e_view    { VIEW_PLOT_3D = 1, VIEW_PLOT_2D, VIEW_EARTH_DAY, VIEW_EARTH_NIGHT, VIEW_EARTH_COMBINED, VIEW_CUBE, VIEW_GAME }; 
// note TRIGGER_DEMO after TRIGGER_ALL so just gets used once
enum e_trigger { TRIGGER_UNSET, TRIGGER_IMMEDIATE, TRIGGER_1MIN, TRIGGER_2MIN, TRIGGER_DEMO };  // TRIGGER_10SEC, TRIGGER_20SEC, TRIGGER_30SEC
enum e_endian  { ENDIAN_LITTLE, ENDIAN_BIG };
enum e_where   { WHERE_MAIN_STARTUP,
                 WHERE_MAIN_PROJPREFS, 
                 WHERE_THREAD_SENSOR_INITIAL_MEAN, 
                 WHERE_THREAD_SENSOR_BASELINE,
                 WHERE_THREAD_SENSOR_TIME_ERROR,
				 WHERE_THREAD_SENSOR_DETACH
				};

// enum of quake types for the globe list
enum e_quake   { QUAKE_CURRENT, QUAKE_WORLD85, QUAKE_DEADLIEST };

// sensor information
// if add sensors or strings here don't forget to update the class CSensor in sensor/csensor.cpp!

// enumerate the various sensor types, we can trickle this int back for easier comparisons
// don't forget to update in the csensor.cpp CSensor constructor
enum e_sensor  { 
                 SENSOR_NOTFOUND = 0,  // 0
                 SENSOR_MAC_PPC_TYPE1, // 1
                 SENSOR_MAC_PPC_TYPE2, // 2
                 SENSOR_MAC_PPC_TYPE3, // 3
                 SENSOR_MAC_INTEL,     // 4
                 SENSOR_WIN_THINKPAD,  // 5
		 SENSOR_WIN_HP,        // 6
		 SENSOR_ANDROID,       // 7
		 SENSOR_USB_JW24F8  = 100,   // 100
		 SENSOR_USB_MOTIONNODEACCEL, // 101
	         SENSOR_USB_ONAVI_1,     // 102
  		 SENSOR_USB_JW24F14,      // 103
	         SENSOR_USB_ONAVI_A_12,   // 104
	         SENSOR_USB_ONAVI_B_16,   // 105
	         SENSOR_USB_ONAVI_C_24,    // 106
	         SENSOR_USB_PHIDGETS_1056, // 107
	         SENSOR_USB_PHIDGETS_1042, // 108
	         SENSOR_USB_PHIDGETS_1044,  // 109
	         SENSOR_USB_PHIDGETS_1041,  // 110
	         SENSOR_USB_PHIDGETS_1043  // 111
               };

// set to the min allowable value of a usb sensor enum as above
#define MIN_SENSOR_USB 100
// set to the max allowable value of a usb sensor enum as above
#define MAX_SENSOR_USB 111


// USB id's
#define USB_MOUSEWARRIOR       0x1114
#define USB_DEVICEID_JW24F8    0x1113
#define USB_DEVICEID_JW24F14   0x1116
#define USB_VENDORID_JW        0x07C0

#define USB_COOKIE_X_JW24F8    0x30
#define USB_COOKIE_Y_JW24F8    0x31
#define USB_COOKIE_Z_JW24F8    0x32

#define USB_COOKIE_X_JW24F14   0x30
#define USB_COOKIE_Y_JW24F14   0x31
#define USB_COOKIE_Z_JW24F14   0x32

// ERR_NO_SENSOR   = no sensor was detected in the CSensor::detect() call
// ERR_SHMEM       = could not create/setup shared memory block for the CQCNShMem class
// ERR_TIME_CHANGE = err in the clocks, user may have changed the clock in the middle of running QCN?
// ERR_INIT        = call to boinc_init failed -- possibly out of memory?
// ERR_CRASH       = an unspecified catch-all for a QCN crash

// the number of seconds lag before we consider it an error an reset the sensor
#define TIME_ERROR_SECONDS .51f         // an error of more than this (2000%!) is critical and we should reset timer
#define ERROR_CUMULATIVE_CUTOFF 1000
#define SLUGGISH_MACHINE_THRESHOLD 100   // if exceeds this many time resets in a session, bump up DT to DT_SLOW

#define THREAD_EXIT -1
#define QCN_SHMEM         "quake"

#define NO_LAT -91.0f
#define NO_LNG -361.0f

#ifdef _WIN32
#define QCN_USB_SENSOR "\\\\.\\pipe\\qcn_usb_sensor"
#define QCN_USB_STATE  "\\\\.\\pipe\\qcn_usb_state"
#else
#ifdef __APPLE_CC__
#define QCN_USB_SENSOR  "/tmp/qcn_usb_sensor"
#define QCN_USB_STATE   "/tmp/qcn_usb_state"
#endif
#endif

struct FDSET_GROUP {
    fd_set read_fds;
    fd_set write_fds;
    fd_set exc_fds;
    int max_fd;

    void zero() {
        FD_ZERO(&read_fds);
        FD_ZERO(&write_fds);
        FD_ZERO(&exc_fds);
        max_fd = -1;
    }
};

#ifndef FALSE
#define FALSE 0
#endif
#ifndef TRUE
#define TRUE  1
#endif
#define WHERE_MAIN 0
#define WHERE_PREFS 10

// ntpdate stuff for sync'ing a BOINC/QCN participant with the qcn-upl time server
// ntpdate exec can be obtained by boinc_resolve_filename, set ntpdate=ntpdate_4.2.4_i686-apple-darwin in boinc/apps/qcnalpha/*
#define NTPDATE_EXEC "ntpdate"
#define NTPDATE_EXEC_VERSION "ntpdate_4.2.6p5a"

#ifdef _WIN32
#ifdef _WIN64
#define BOINC_WIN_SUFFIX "windows_x86_64"
#else
#define BOINC_WIN_SUFFIX "windows_intelx86"
#endif
#endif

// the args used for ntpdate call for 8 samples, a timeout of 20 seconds, an unprivileged port, no adjtime, and no system update of the clock
#ifdef _USE_NTPDATE_EXEC_
  #define NTPDATE_ARGS "-b -t 20 -q pool.ntp.org"
  #define NTPDATE_ARGC 5
#else
  #define NTPDATE_ARGS { "-b -t", "20", "-q", "pool.ntp.org" }
  #define NTPDATE_ARGC 5
#endif

// search strings to parse in the reply for ntpdate
#define NTPDATE_STR_SEARCH_1 "time server"
#define NTPDATE_STR_SEARCH_2 "ffset "
#define NTPDATE_STR_SEARCH_3 "sec"

// time to wait if they were in interactive mode before sending a trigger, timeout in 60 seconds
#define DECAY_INTERACTIVE_SECONDS 60.0f

// how often to write out trigger file if in demo mode 
#ifdef QCN_RAW_DATA
  #define DEMO_TRIGGER_TIME_SECONDS 3600.0f
#else
  #define DEMO_TRIGGER_TIME_SECONDS 600.0f
#endif

// number of elements in the shared mem time offset arrays
//#define MAX_TIME_ARRAY    10 

#define MAX_TRIGGER_LAST  100
#define MAX_TICK_MARK      13

// CMC -- or perhaps trickle down to a slots qcn-quake.xml file?
//#define XML_QUAKE_FILE  "qcn-quake.xml"
#define XML_PREFS_FILE    "qcnprefs.xml"
#define STDERR_FILE       "stderr.txt"
#define XML_TRIGGER_COUNT "trigcnt"
#define XML_UPLOAD_COUNT  "uplcnt"
#define XML_RESET_COUNT   "rstcnt"
#define XML_CLOCK_TIME    "wct"
#define XML_CPU_TIME      "cpt"
#define XML_SENSOR        "sms"
#define DIR_TRIGGER       "triggers"
#define DIR_CONTINUAL     "continual"

// 50 uploads max for continual, 20 for regular, default to 50
#define MAX_UPLOAD        50

// delete files over a month old
#define TIME_FILE_DELETE  2592000.0f
#define MAIN_LOOP_SLEEP_MICROSECONDS 200000L
#define MAIN_LOOP_SLEEP_SECONDS 0.2f

// check trickledown and quake list about every hour (1/2 hour = 1800 seconds, i.e. when loop gone through .2sec * 9,000 times)
// note this depends on the above value for MAIN_LOOP_SLEEP_MICROSECONDS!
//#define COUNTER_CHECK   9000L
//#define COUNTER_TRICKLE 11000L

// check for clock time offset every 15 minutes
#define TIME_CHECK      10000L
#define MAX_NUM_RESET    1000L     // maximum number of reset errors per workunit
#define TIME_BACK_SECONDS_MAX 7200L  // max number of minutes to go back from the GUI

#define SAC_NUMHEADBYTES 632L  // floats and longs are 440 rest are characters 
#define SAC_VERSION 6L

#define QCN_BYTE unsigned char
#define QCN_CBYTE const unsigned char

//#define MAXI 150000L   // this is 4 hours 10 minutes at dt of .1 seconds 
//#define MAXI 360000L   // this is 2 hour at dt of .02 seconds
//#define MAXI 720000L   // this is 4 hours at dt of .02 seconds
#define MAXI 270000L     // this is 1.5 hour at dt of .02 seconds

#ifndef _MAX_PATH
   #define _MAX_PATH 255
#endif
#define _MAX_PATH_LOGICAL 20
#define SIZEOF_CHECKSUM 33

#define TRICKLE_DOWN_ABORT        "<abort>"
#define TRICKLE_DOWN_FILELIST     "<filelist>"
#define TRICKLE_DOWN_QUAKE        "<quakes>"
#define TRICKLE_DOWN_SEND         "<sendme>"
#define TRICKLE_DOWN_SEND_END_TAG "</sendme>"

// this is the max number of the array index awinsize used to shift the graphics window 
#define MAX_KEY_WINSIZE 3
// the reduction/rebin for the data array size for the graphics
#define PLOT_ARRAY_SIZE 500

// this is used for the boinc_sleep interval for a sensor, it's basically dt/SAMPLING_FREQUENCY each sleep interval
// which seems to yield 5-10 samples per dt
// this is DT/SAMPLING_FREQUENCY * 1e6
#define DT_MICROSECOND_SAMPLE 2000
#define SAMPLING_FREQUENCY  10.0f
#define SAMPLE_SIZE 5

#ifdef GRAPHICS_PROGRAM

#define MAX_PLOT_HEIGHT      8.0f
#define MAX_PLOT_HEIGHT_QCNLIVE      7.5f
#define TEXT_PLOT_LEFT_AXES .23f
//#define TEXT_PLOT_LEFT_AXES .03f
#define QUAKE_MAGNITUDE_FACTOR 300.0f
#define QUAKE_AUTOROTATE_SCALE 30

// array data for graphics
#define NUM_COASTLINE          18441
#define NUM_COUNTRY              268
#define NUM_NATION_BOUNDARY    24792
#define NUM_BDY_EUROPE         27856
#define NUM_BDY_AFRICA         27818
#define NUM_BDY_SOUTH_AMERICA  27687
#define NUM_BDY_ASIA           27546
#define NUM_BDY_NORTH_AMERICA  23208
#define NUM_PLATE_BOUNDARY      5706

// OpenGL text draw sizes
#define MSG_SIZE_BIG    1200
#define MSG_SIZE_NORMAL 1400
#define MSG_SIZE_MEDIUM 1600
#define MSG_SIZE_SMALL  2000
#define MSG_SIZE_TINY   2400

#define MAX_VXP 4
enum e_drawtype { NATION = 0, PLATE, COUNTRY, COASTLINE };

// earth map stuff
/*
#define IMG_EARTH_DAY   "earth_" QCN_VERSION_STRING ".jpg"
#define IMG_EARTH_NIGHT "earth_night_" QCN_VERSION_STRING ".jpg"
#define IMG_LOGO        "logo_" QCN_VERSION_STRING ".jpg"
*/

// don't need version nums, but if change file should rename!
#if defined(_WIN32) || defined(__APPLE_CC__)
#  define IMG_EARTH_DAY         "earthday4096.jpg"
#  define IMG_EARTH_NIGHT       "earthnight4096.jpg"
#else
//   Linux gets smaller pics as can't figure out why the jpeg doesn't show on Linux OpenGL
//   other than a smaller pic works!  It's not stack size as I've set it up high and still 
//   does not display on Linux
#  define IMG_EARTH_DAY         "earthday2048.jpg"
#  define IMG_EARTH_NIGHT       "earthnight2048.jpg"
#endif

#define IMG_EARTH_MASK        "earthmask.rgb"
#define TEXTURE_X 4096
#define TEXTURE_Y 2048
#define IMG_LOGO         "logo_iris.jpg"
#define IMG_LOGO_EXTRA   "ad.jpg"
#define IMG_LOGO_XYZAXES "xyzaxes.jpg"
#define IMG_LOGO_XYZAXES_BLACK "xyzaxesbl.jpg"

#define PI 3.14159265f

#define URL_USGS "http://www.usgs.gov"

#define EARTH_LON_RES   90      // Longitude Resolution (x)
#define EARTH_LAT_RES   90      // Latitude Resolution (y)
#define EARTH_TILT_ANGLE  23.445249   // axial tilt in degrees using Int'l Astro Union  http://en.wikipedia.org/wiki/Axial_tilt#Values
#define EARTH_RADIUS    1.0
//#define EARTH_RADIUS    6378.0    // in kilometers
#define WORLD_SCALE             0.01f   // scale of all the world
#define MIN_SCALE               0.5f    // how much can we zoom out
#define MAX_SCALE               10.0f    // and in
#define LIGHT_INFINITY     -1000.0f  // light source is almost at infinity
#define ROTATION_SPEED_DEFAULT 0.5f 

#endif // GRAPHICS_PROGRAM

#endif

