/*
 *  main.cpp
 *  qcn
 *
 *  Created by Carl Christensen on 11/08/2007
 *
 */

#include "main.h"
#include "boinc_zip.h"

// don't use customized boinc_options if qcnlive, the gui will control the thread quit etc
#ifndef QCNLIVE
#define __USE_BOINC_OPTIONS  // use customized boinc options
#endif

// the main entry point for the BOINC app (not the wxWidgets QCNLIVE)
// for Windows non-wxWidgets builds (i.e. a "normal" BOINC Windows system build)
// we want a WinMain proc which is the Windows entry point to main()
// note the only thing global (outside of a namespace) is our sm shared mem class

#ifndef QCNLIVE

CQCNShMem* volatile sm = NULL;

/*
  // CMC used to send uploads from this (main) thread, but seems OK to leave in the sensor thread since it's at the end of a monitoring session
  void checkForUpload()
  {
        if (sm 
           && strlen(sm->strUploadResolve)>1 
           && strlen(sm->strUploadLogical)>1 
           && boinc_file_exists(sm->strUploadResolve)) { // send this file, whether it was just made or made previously
             // note boinc_upload_file (intermediate uploads) requires the logical boinc filename ("soft link")!
             qcn_util::sendIntermediateUpload(sm->strUploadLogical, sm->strUploadResolve);  // the logical name gets resolved by boinc_upload_file into full path zip file 
             memset(sm->strUploadResolve, 0x00, sizeof(char) * _MAX_PATH);
             memset(sm->strUploadLogical, 0x00, sizeof(char) * _MAX_PATH_LOGICAL);
        }
  }
*/

  // dummy wrapper fn for doMainQuit
  void globalQuit()
  {
      qcn_main::doMainQuit();
  }

  int main(int argc, char **argv) 
  {
     // install signal handlers here 
//#ifndef _DEBUG
     qcn_signal::InstallHandlers(qcn_main::signal_handler);   // note this is set to ignore SIGPIPE by default
    // qcn_signal::InstallHandlerSIGPIPE(qcnmain_sigpipe);
//#endif

/*
#ifdef _WIN32 // we're in console mode if here, set title
    char* strTitle = new char[32];
    memset(strTitle, 0x00, 32);
    sprintf(strTitle, CONSOLE_TITLE_FORMAT, ::GetCurrentProcessId());
    ::SetConsoleTitle(strTitle);
    delete [] strTitle;
#endif
*/
     atexit(globalQuit);
     return qcn_main::qcn_main(argc, argv); // run the main qcn program
  }
  #ifdef _WIN32   // transfer WinMain call in Windows to our regular main()
  int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPSTR Args, int WinMode) {
      LPSTR command_line;
      char* argv[100];
      int argc;

      command_line = GetCommandLine();
      argc = parse_command_line( command_line, argv );
      return qcn_main::qcn_main(argc, argv);
  }
  #endif  // _WIN32
#endif // ndef QCNLIVE

namespace qcn_main  {

// global variables (within namespace qcn_main of course)

  CSensor* volatile g_psms = NULL; // pointer to the CSensor - so we can closePort() on a signal error
  e_endian volatile g_endian;
	
  // use volatile keyword as these are used across threads for status changes etc
  int volatile g_iStop = FALSE;  // false;   note changed to int because used in a C program (ntpdatemain) which doesn't have bool
  bool volatile g_bDetach = false; // sensor detached
  bool volatile g_bFinished   = false;
  bool volatile g_bSuspended  = false;
  int  volatile g_iQCNReturn  = 0; // qcn return code

  CQCNThread* volatile g_threadSensor = NULL;
  CQCNThread* volatile g_threadTime = NULL;
  CQCNThread* volatile g_threadMain = NULL;

  //unsigned int g_aiPhidget[MAX_NUM_PHIDGETS];

  float g_fPerturb[MAX_PERTURB];  // our potential perturbation points (just 2 used now - sig cutoff, and avg short-term )

  bool g_bReadOnly = false;

  // simple flag to denote continual app
  bool g_bContinual = false;  // default to not running in continual mode i.e. 10 minute output

  bool g_bDemo = false;
	
#if QCNLIVE
	const bool g_bQCNLive = true;
#else
	const bool g_bQCNLive = false;
#endif
  char g_strPathTrigger[_MAX_PATH] = {""};  // this is the path to trigger, doesn't change after startup
  char g_strPathContinual[_MAX_PATH] = {""}; // path for continual stuff

  double g_dTimeOffset = 0.0f;  // the time offset between client & server, +/- in seconds difference from server
  double g_dTimeSync = 0.0f;    // the (unadjusted client) time this element was retrieved
  double g_dTimeSyncRetry = 0.0f; // retry time for the time sync thread
  double g_dTimeCurrent  = 0.0f;   // current time in the main thread, so we can check in half-hour for trigger trickle etc
  double g_dTimeNextPing = 0.0f;   // time of the next ping, if g_dTimeCurrent is greather than this, time for a trigger trickle
  bool   g_bFirstPing = false;     // flag that we did the first ping trickle

  vector<struct STriggerInfo> g_vectTrigger;  // a list for all the trigger info, use bTriggerLock to control access for writing

void checkContinualUpload(bool bForce = false);

// function to send the final trigger as the workunit is done either normal or via a crash or abort
void sendFinalTrickle()
{
      if (sm && sm->clock_time>0.0f) {  // send a final trickle if they have clock_time to report
        char strFinal[_MAX_PATH];
        memset(strFinal, 0x00, _MAX_PATH);   // the sprintf below is from set_qcn_counter() in client/util/qcn_util.cpp
        sprintf(strFinal, 
          "<%s>%d</%s>\n"
          "<%s>%d</%s>\n"
          "<%s>%d</%s>\n"
          "<%s>%.2f</%s>\n"
          "<%s>%.2f</%s>\n",
          XML_TRIGGER_COUNT, sm->iNumTrigger, XML_TRIGGER_COUNT,
          XML_UPLOAD_COUNT, sm->iNumUpload, XML_UPLOAD_COUNT,
          XML_RESET_COUNT, sm->iNumReset, XML_RESET_COUNT,
          XML_CLOCK_TIME, sm->clock_time, XML_CLOCK_TIME,
          XML_CPU_TIME, sm->cpu_time, XML_CPU_TIME
        );
        // upload the qcn_prefs.xml file
        trickleup::qcnTrickleUp(strFinal, TRIGGER_VARIETY_FINALSTATS, (const char*) sm->dataBOINC.wu_name);  // trickle the final stats, basically like uploading qcnprefs.xml file
      }
}

// simple function to set stop state & wait for threads to quit
void doMainQuit(const bool& bFinish, const e_retcode& errcode)
{
  static bool bDone = false;
  if (bDone) return; // already did this, so if we are in here, then atexit() is invoked, it won't go through a second time
  bDone = true;
  if (sm) sm->eStatus = errcode;
  g_iStop = TRUE; // global flag to stop
  g_bFinished = bFinish;
  g_iQCNReturn = (int)(errcode == ERR_FINISHED ? ERR_NONE : errcode);  // note a timeout (ERR_FINISHED) error is "normal" exit
  if (g_threadTime) g_threadTime->Stop();
  if (g_threadSensor) g_threadSensor->Stop();
  int iStopCtr = 0;

  //fprintf(stdout, "Quitting QCN...\n");
  while (g_threadSensor && g_threadTime
	   && (g_threadSensor->IsRunning() || g_threadTime->IsRunning()) 
	   && iStopCtr++ < 3000) { // wait up to 3 seconds, I believe BOINC gives us 5 seconds to quit
     usleep(1000L);
  }
  if (g_threadTime) {
        delete g_threadTime;
        g_threadTime = NULL;
  }
  if (g_threadSensor) {
        delete g_threadSensor;
        g_threadSensor = NULL;
  }
   qcn_util::set_qcn_counter(); // write our settings to disk
   // free project prefs
   if (sm && sm->dataBOINC.project_preferences) {
	   free(sm->dataBOINC.project_preferences);
	   sm->dataBOINC.project_preferences = NULL;
   }
  fprintf(stderr, "qcn_main: waited %f seconds for sensor and time threads to end\n", iStopCtr == 0 ? 0.0 : .001f * (float) --iStopCtr);
}

// CMC try to send a final trickle & call boinc_finish?  or is this stuff too dangerous in a signal handler?
// install signal handlers
void signal_handler(int iSignal)
{
   static bool bHere = false;
   if (bHere) return;
   bHere = true;
   g_iStop = TRUE; // flag that we're stopping everything
   fprintf(stderr, "Signal %d received, exiting...\n", iSignal);
   if (!boinc_is_standalone()) sendFinalTrickle(); // try to send final trigger, or is this dangerous in a signal handler?
   doMainQuit(true, ERR_SIGNAL); // close up threads
   if (g_psms) {
      // send a message to the sensor thread to shutdown
      fprintf(stderr, "Closing accelerometer port due to signal...\n");
      g_psms->closePort();
      g_psms = NULL;
   }
   if (boinc_is_standalone())  // standalone, maybe QCNLive, just exit
      _exit(EXIT_SIGNAL);
   else  // live in BOINC - finish this workunit
      boinc_finish(EXIT_SIGNAL); // this never returns and exits the app
}

/*
void qcnmain_sigpipe(int iSignal)
{
   fprintf(stderr, "SIGPIPE received, resetting\n");
   //if (smState) smState->bStop = true;
}
*/

void update_sharedmem() 
{
    static double dLastTime = 0.0f;
    g_dTimeCurrent = dtime();
    sm->current_time = g_dTimeCurrent;
    if (g_iStop || !sm) return;
    boinc_get_status(&sm->statusBOINC);

	if (!g_bSuspended) { // use the global suspended flag, since it will go through once anyway to set the suspend status
		sm->fraction_done = boinc_get_fraction_done();
        if (sm->bSensorFound) { 
            sm->update_time = g_dTimeCurrent;
            // important -- only count time if they have a sensor!
            // otherwise any idiot who wants easy credits can just run QCN with no sensor
            boinc_wu_cpu_time((double&) sm->cpu_time);  // this is the entire workunit cpu time, which we want
            if (dLastTime == 0.0f) { // we just started, so add the main loop time
                sm->clock_time += ((float)MAIN_LOOP_SLEEP_MICROSECONDS / 1.0e6f); // increment our clock time counter
            }
            else { // we can just use the difference of dLastTime & update_time
                sm->clock_time += (sm->update_time - dLastTime);
            }
            dLastTime = sm->update_time;  // set the time to get the delta above
        }
    }
}

/* 
   example BOINC checkpoint call -- QCN doesn't really need something like this since we
   do real-time monitoring of a sensor -- but we'll probably want a nice way to stream out the shared-mem to disk
   for certain ranges, i.e. trigger, or always want to stream if comparing with seismograph etc 
*/
int do_checkpoint(MFILE& mf, int nchars) {
    return 0;
}

int qcn_main(int argc, char **argv)
{
    g_iQCNReturn = 0;

    // first test endian-ness
    g_endian = qcn_util::check_endian(); // g_endian is global to all procs, i.e. the sac file I/O utils can now use it
    fprintf(stdout, "Host machine is %s-endian\n", g_endian == ENDIAN_BIG ? "big" : "little");
#ifdef QCN_RAW_DATA
    fprintf(stdout, "Note: Running in 'raw data' mode (integer values from the accelerometer)\n");
#endif
    fflush(stdout);

#ifdef __USE_BOINC_OPTIONS
    BOINC_OPTIONS optBOINC;
    optBOINC.main_program = true;
    optBOINC.direct_process_action = false;  // we'll handle quit/suspend/resume requests from BOINC
    optBOINC.check_heartbeat = true;
    optBOINC.handle_process_control = true;
    optBOINC.handle_trickle_ups   = true;
    optBOINC.handle_trickle_downs = true;
    optBOINC.send_status_msgs = true;
    //optBOINC.worker_thread_stack_size = 1048576L;
    //optBOINC.backwards_compatible_graphics = true;
    g_iQCNReturn = boinc_init_options(&optBOINC); // use our own options & start the boinc init
#else
    g_iQCNReturn = boinc_init(); // start the boinc init
#endif

    if (g_iQCNReturn) return ERR_INIT; // return init error

#ifndef QCNLIVE
    // create shared mem segment for data & graphics -- if running the GUI this is done in the main GUI app (i.e. gui/qcnmac.cpp)
    sm = static_cast<CQCNShMem*>(boinc_graphics_make_shmem((char*) QCN_SHMEM, sizeof(CQCNShMem)));
    if (sm) {
        g_bDemo = (bool) boinc_is_standalone(); // ? true : false;
        parseArgs(argc, argv); // parse command line arguments, very important to set g_bContinual for ResetCounter below (i.e. to get paths)
        qcn_util::ResetCounter(WHERE_MAIN_STARTUP);  // this is the one and only place ResetCounter is called outside of the sensor thread, so it's safe
    }
    else {
        fprintf(stderr, "failed to create shared mem segment\n");
        fprintf(stdout, "failed to create shared mem segment\n");
        return ERR_SHMEM;
    }
#endif  // QCNLIVE sets up it's own memory and does it's own parseArgs & ResetCounter

/* move update_sharedmem to our main while loop which is 5Hz
    if (sm && !sm->bReadOnly) {
        // setup shared memory for graphics and set the callback function for graphics data to get updated
        // note graphics is in a separate program which uses the shared memory to look at the data
        update_sharedmem();
        //boinc_register_timer_callback(update_sharedmem);   // this means update_sharedmem will be automatically called once per second
    }
*/

#ifdef _DEBUG
   if (strlen(g_strPathTrigger)<1) {
     strcpy(g_strPathTrigger, "../../data");
   }
#endif

    // make our trigger zip data dir if it doesn't exist
    if (strlen(g_strPathTrigger)>1 && ! boinc_file_exists((const char*) g_strPathTrigger) ) {
       // now open dir to see if exists
       if (boinc_mkdir((const char*) g_strPathTrigger)) {
          // OK, now it's a fatal error
          fprintf(stderr, "QCN exiting, can't make directory %s\n", g_strPathTrigger);
          fprintf(stdout, "QCN exiting, can't make directory %s\n", g_strPathTrigger);
          fflush(stdout);
          return ERR_DIR_TRIGGER;
       }
    }
	if (g_bContinual) {
#ifdef _DEBUG
   if (strlen(g_strPathContinual)<1) {
     strcpy(g_strPathContinual, "../../data_continual");
   }
#endif
		if (strlen(g_strPathContinual)>1 && ! boinc_file_exists((const char*) g_strPathContinual) ) {
		   // now open dir to see if exists
		   if (boinc_mkdir((const char*) g_strPathContinual)) {
			  // OK, now it's a fatal error
			  fprintf(stderr, "QCN exiting, can't make directory %s\n", g_strPathContinual);
			  fprintf(stdout, "QCN exiting, can't make directory %s\n", g_strPathContinual);
			  fflush(stdout);
			  return ERR_DIR_TRIGGER;
		   }
		}
	}
   
    // make our images directory -- this is stored in sm->strPathImage
    if (sm->strPathImage && ! boinc_file_exists((const char*) sm->strPathImage) ) {
       // now open dir to see if exists
       if (boinc_mkdir((const char*) sm->strPathImage)) {
          // OK, now it's a fatal error
          fprintf(stderr, "QCN exiting, can't make directory %s\n", sm->strPathImage);
          fprintf(stdout, "QCN exiting, can't make directory %s\n", sm->strPathImage);
          fflush(stdout);
          return ERR_DIR_IMAGES;
       }
    }

// don't perturb now
#if 0
    // now get the input file (if we're not in demo mode, i.e. running "live" under BOINC)
    if (!g_bDemo && !g_bQCNLive)  {
      char strData[_MAX_PATH], strResolve[_MAX_PATH];
      memset(strResolve, 0x00, _MAX_PATH);
      memset(strData, 0x00, _MAX_PATH);
      if (boinc_resolve_filename(QCN_INPUT_LOGICAL_NAME, strResolve, _MAX_PATH) && !strResolve[0]) {
        // this name didn't resolve, return!
        fprintf(stderr, "Input file %s not resolved!\n", QCN_INPUT_LOGICAL_NAME);
        return ERR_INPUT_FILE;
      }

      char* tmpbuf = NULL;
      double dTemp = 0.0f;
      // OK, we resolved the file, so let's open & parse it
      if (!read_file_malloc(strResolve, tmpbuf) && tmpbuf) {
         if (!parse_double(tmpbuf, XML_SIG_CUTOFF, dTemp)) return ERR_INPUT_PARSE;  // make it a fatal error if can't parse input file
         g_fPerturb[PERTURB_SIG_CUTOFF] = (float) dTemp;

         if (!parse_double(tmpbuf, XML_SHORT_TERM_AVG_MAG, dTemp)) return ERR_INPUT_PARSE;
         g_fPerturb[PERTURB_SHORT_TERM_AVG_MAG] = (float) dTemp;
            
         if (tmpbuf) free(tmpbuf);
      }
    } // end getting the input file data if not in demo mode
#endif

/*
#if defined(__LINUX_ARMV5__) || defined(__LINUX_ARMV6__)
   // copy over the libusb shared object
   const char* strProjPath = (const char*) sm->dataBOINC.project_dir;
   char* strOldSO = new char[_MAX_PATH];
   char* strNewSO = new char[_MAX_PATH];
   sprintf(strOldSO,"%s/%s", strProjPath, "libusb-arm-1.0.so");
   sprintf(strNewSO,"%s/%s", strProjPath, "libusb-1.0.so");
   if (!boinc_file_exists(strNewSO)) {
     boinc_copy(strOldSO, strNewSO);
   }
   delete [] strOldSO;
   delete [] strNewSO;
#endif
*/

    // print out our values just as a "sanity check"
    fprintf(stdout, "Significance Filter Cutoff    = %f\n", g_fPerturb[PERTURB_SIG_CUTOFF]);
    fprintf(stdout, "Short Term Average Magnitude  = %f\n", g_fPerturb[PERTURB_SHORT_TERM_AVG_MAG]);
    fprintf(stdout, "BOINC Project Path            = %s\n", (const char*) sm->dataBOINC.project_dir);
    fflush(stdout);
 
    // OK, if not in demo mode, now get rid of old trigger files i.e. more than two weeks old
    if (g_bContinual)  { // get rid of files older than a day if running in continual mode
       qcn_util::removeOldTriggers((const char*) g_strPathTrigger, 86400.0f);
    }
    else {
      if (!g_bDemo && !g_bQCNLive) qcn_util::removeOldTriggers((const char*) g_strPathTrigger);  // default is get rid of files older than a month
    }

    // create time & sensor thread objects
    //sm->bFlagUpload = false;
    //memset(sm->strFileUpload, 0x00, sizeof(char) * _MAX_PATH);
    g_threadSensor = new CQCNThread(QCNThreadSensor);
    if (!g_threadSensor) {
       fprintf(stderr, "QCN exiting, can't create sensor thread\n");
       fprintf(stdout, "QCN exiting, can't create sensor thread\n");
       return ERR_CREATE_THREAD;
    }

    g_threadTime = new CQCNThread(QCNThreadTime);
    if (!g_threadTime) {
       fprintf(stderr, "QCN exiting, can't create time sync thread\n");
       fprintf(stdout, "QCN exiting, can't create time sync thread\n");
       return ERR_CREATE_THREAD;
    }

    // since we passed all the major return points, mark the thread as running
    if (g_threadMain) g_threadMain->SetRunning(true);  // self-referential pointer to flag the thread is running 

    // now start the main loop
    while (true) 
    { // this is a simple main idle loop, the sensor is off being monitored in a thread (or not, if we're just in demo mode!)
        update_sharedmem();
        if (g_iStop) {
           doMainQuit();
           goto done; 
        }
#ifdef __USE_BOINC_OPTIONS
        if (sm->statusBOINC.quit_request) {
           fprintf(stderr, "Quit request from BOINC!\n");
	   // if we were suspended, then quitting, bump up iNumReset if needed
	   // since it was decremented in the suspend
           // if (g_bSuspended && (g_dTimeCurrent - sm->update_time) > TIME_ERROR_SECONDS) sm->iNumReset--;
           doMainQuit();
           goto done; 
        }
        else  {
          if (sm->statusBOINC.suspended) {
             // check current local suspend flag
#ifndef _DEBUG
             if (!g_bSuspended) { // mark that we're suspended if not already set
                fprintf(stderr, "Suspend request from BOINC!\n");
                g_bSuspended = true;
                g_threadSensor->Suspend();
                g_threadTime->Suspend();
                // now suspend just quits - but eventually should sleep the threads & resume later
             }
#endif
          }
          else { // not suspended, check if we think we're suspended!
#ifndef _DEBUG
             if (g_bSuspended) { // mark that we're resuming
                fprintf(stderr, "Resume request from BOINC!\n");
                g_bSuspended = false;
                g_threadSensor->Resume();
                g_threadTime->Resume();
                // rollback the iNumReset counter by one, since this will obviously reset
                // if it's over our time limit
                if ((g_dTimeCurrent - sm->update_time) > TIME_ERROR_SECONDS) sm->iNumReset--;
             }
             else {
#endif
                if (sm->statusBOINC.no_heartbeat) { // quit non-fatally
                   fprintf(stderr, "No heartbeat from BOINC!\n");
                   doMainQuit();
                   goto done; 
                }
                else {
                   if (sm->statusBOINC.abort_request) { // quit fatally
                     sm->eStatus = ERR_ABORT; // set abort flag so it can be properly handled below
                     fprintf(stderr, "Abort request from BOINC!\n");
                     doMainQuit(true, ERR_ABORT);
                     goto done; 
                   }
                }
#ifndef _DEBUG
             }
#endif
          }
        }

        // if we're suspended then sleep a bit and continue in the loop to poll for unsuspend (or quit)
        if (g_bSuspended) {
            usleep(MAIN_LOOP_SLEEP_MICROSECONDS);
            continue;
        } 
#endif  // __USE_BOINC_OPTIONS

        if (!g_iStop && !g_bReadOnly && !g_threadSensor->IsRunning()) {
           // we're not in readonly mode, and sensor thread hasn't started -- note we run the sensor in bDemo mode!
           // do the thread, note the bool in Start() -- this signifies that the thread should run at higher priority
           // the sensor thread should run at higher priority so as to minimize "dropouts" from the high frequency
           // 50-500Hz sensor hardware access
           if (!g_threadSensor->Start(true)) { // Start() shows that the thread was made OK
           //if (!g_threadSensor->IsRunning()) { // IsRunning() shows that we made it to inside of the thread which set the bool to true
              fprintf(stderr, "Sensor thread failed to start\n");
           }
           else {  
              int iStartup = 0;
              while (! g_threadSensor->IsRunning() && iStartup++<1000) {
                    if (g_iStop) goto done;
                    usleep(1000L);
              }
           }
        }

        // Server Time Synchronization
        // if 15 or so minutes has passed, or the sensor thread requests it (i.e. timing reset error), 
        // do a timing sync with the server
        // maybe pass in the server as a command-line argument in case we switch or mirror?
        // note -- try to do time check in demo mode, but not readonly mode
        // added a sensor thread, found, and object check, i.e. no need to check time if no sensor, right?
        if (!g_iStop && sm && !g_bReadOnly && !g_threadTime->IsRunning()
          && g_threadSensor->IsRunning() && sm->bSensorFound && g_psms
          && g_dTimeSyncRetry < sm->update_time) {  
           // we're not in readonly/demo mode, and just starting or it's time to check

           // do the thread
           if (!g_threadTime->Start()) { // Start() shows that the thread was made OK
           //if (!g_threadTime->IsRunning()) { // IsRunning() shows that we made it to inside of the thread which set the bool to true
              fprintf(stderr, "Time sync thread failed to start\n");
           }
           else {  
              int iStartup = 0;
              while (! g_threadTime->IsRunning() && iStartup++<1000)  {
                    if (g_iStop) goto done;
                    usleep(1000L);
              }
           }
        }

        // at this point the sensor should be monitored in a separate thread (and suspend/resume/quit via BOINC calls)
        // so this will be a simple loop where we can check status for triggers etc

        // check boinc checkpoint time, this is fast so we can call every .2 seconds
        if (boinc_time_to_checkpoint()) boinc_checkpoint_completed();

        if (!g_iStop && !g_bDemo && !g_bQCNLive && !g_bReadOnly)  { // only active BOINC workunits need to do the following (i.e. not demo/readonly)

          // Trickle Down & Ping Trickle Check -- i.e. for file requests or to abort workunit
          // check every half-hour
          // unnecessary for demo mode
          if (g_dTimeNextPing < g_dTimeCurrent) { // time for a ping trickle & trickle down check
			  double dTimeLastTrigger = 0.0;
			  long lTimeLastTrigger, lTriggerCount;
			  lTriggerCount = qcn_util::getLastTrigger(dTimeLastTrigger, lTimeLastTrigger);
			  
              g_dTimeNextPing = g_dTimeCurrent + INTERVAL_PING_SECONDS;  // set the next interval

			  // a good point to check for trickle down i.e. file request
			  trickledown::processTrickleDown();  // from util/trickledown.cpp

             // see if we have an intermediate upload
//#ifndef QCNLIVE
//             checkForUpload();
//#endif

             // this is a good spot to check for file uploads
             checkContinualUpload();

             // this is also a good spot to check for massive numbers of resets (time adjustments) for this workunit
             if (sm->iNumReset > MAX_NUM_RESET) { // this computer sucks, trickle up and exit workunit
                 fprintf(stderr, "Too many resets (%d) for this workunit and host, exiting...\n", sm->iNumReset);
                 doMainQuit(true, ERR_NUM_RESET);
                 goto done;
             }

             // New Quake List -- now done with trickle down request about every 30 minutes
             // check for new quake list every hour, i.e. when mod 1000
             // unnecessary for demo mode -- but should we wget or curl the latest quake list, perhaps just in the ./runme script?
             //if (!g_iStop && !(++iQuakeList % QUAKELIST_CHECK))  {

			  // OK now send the ping trickle if we haven't heard from this machine in a half-hour
              // this skips the very first time in i.e. when we're just starting up, also bypasses for the continual reporting app
			  // also make sure we didn't just have a trickle within the past INTERVAL_PING_SECONDS (30 minutes)
			  // i.e. don't bother sending a ping since we heard from this machine anyway!
			  
             // actually let's do a ping to get the host lat/lng info and earthquake list - if there is a sensor!
	     if (!g_iStop && g_bFirstPing) { // && !g_bContinual && (dTimeLastTrigger + INTERVAL_PING_SECONDS < g_dTimeCurrent)) {
                char *strTrigger = new char[512];
                double dTrigTimeOffset = g_dTimeSync>0.0f ? g_dTimeOffset : 0.0f;
                boinc_begin_critical_section();
                memset(strTrigger, 0x00, sizeof(char) * 512);
                sprintf(strTrigger, 
                    "<vr>%s</vr>\n"
                    "<os>%s</os>\n"
                    "<sms>%d</sms>\n"
                    "<reset>%d</reset>\n"
                    "<dt>%f</dt>\n"
                    "<ctime>%f</ctime>\n"
                    "<tsync>%f</tsync>\n"
                    "<toff>%f</toff>\n"
                    "<%s>%.2f</%s>\n"
                    "<%s>%.2f</%s>\n",
                    QCN_VERSION_STRING,
                    qcn_util::os_type_str(),
                    sm->eSensor,
                    sm->iNumReset,
                    sm->dt,
                    g_dTimeCurrent + dTrigTimeOffset, // note we're sending the local client offset sync time adjusted to server time!
                    g_dTimeSync    + dTrigTimeOffset, // adj sync time with the offset if exists
                    g_dTimeOffset,
                    XML_CLOCK_TIME, sm->clock_time, XML_CLOCK_TIME,
                    XML_CPU_TIME, sm->cpu_time, XML_CPU_TIME
                );
                trickleup::qcnTrickleUp(strTrigger, TRIGGER_VARIETY_QUAKELIST, (const char*) sm->dataBOINC.wu_name);  // request a new quake list
                delete [] strTrigger;
                boinc_end_critical_section();
              }
              if (!g_bFirstPing) g_bFirstPing = true; // set this flag so it will do a ping trickle next time through
          } // trickle down check/processing as well as quakelist/ping

          // Parse Prefs of New Quake List
          if (!g_iStop && sm->statusBOINC.reread_init_data_file)  { // should have gotten a quakelist by now
             sm->statusBOINC.reread_init_data_file = 0;
             fprintf(stderr, "qcn_main::BOINC requested reread of init data file at %f\n", g_dTimeCurrent);
             qcn_util::getBOINCInitData(WHERE_MAIN_PROJPREFS);  // reread project_prefs since may have changed, send in 1 to fake it's in a thread so doesn't reset anything else
          }

        }

        // if done 1 wall-clock day, or we got an abort request (i.e. trickle-down msgs), then quit workunit
        if (!g_bDemo && !g_bQCNLive && !g_bReadOnly 
          && (sm->clock_time >= WORKUNIT_COMPLETION_TIME_ELAPSED || sm->eStatus == ERR_ABORT)) {
            // seems to be a race condition upon a quit, so just exit
            doMainQuit(true, (sm->eStatus == ERR_ABORT ? ERR_ABORT: ERR_FINISHED)); // do the little timer loop
            if (g_bContinual)  {
                 usleep(1e6); // wait a second for thread cleanup?
                 fprintf(stderr, "Main thread - force final trigger check - trigcnt=%d\n", (int) g_vectTrigger.size());
                 CheckTriggers(true);  // do one last trigger if needed
            }
            goto done;
        }
        else { // update fraction done 
            boinc_fraction_done((float) sm->clock_time / WORKUNIT_COMPLETION_TIME_ELAPSED);
            CheckTriggers(false);
        }

        usleep(MAIN_LOOP_SLEEP_MICROSECONDS); // 5Hz should be fast enough to monitor things such as trickles, stop/suspend requests etc
        //boinc_sleep(MAIN_LOOP_SLEEP_SECONDS);

	/* test the memory leaks from the BOINC parse_init_data()
#ifdef _DEBUG
		if (!(++iQuakeList % 30)) {
          for (int i = 0; i < 3000; i++) qcn_util::ResetCounter(WHERE_MAIN_PROJPREFS, 0);
		  doMainQuit(false, 0);
		  goto done;
		}
#endif
	*/

	} 

done:
#ifndef QCNLIVE
    // see if we have an intermediate upload
//    checkForUpload();
    if (g_bFinished)  { // not a requested exit, we must be done this workunit
      checkContinualUpload(true);  // note that the doMainQuit would have stopped the sensor thread and marked the final trigger which we'll now process
      sendFinalTrickle();
      boinc_fraction_done(1.00);
      update_sharedmem();  // final call
      fprintf(stdout, "End of workunit\n");
      fflush(stdout);
      fprintf(stderr, "End of workunit\n");
      fflush(stderr);
      if (g_threadMain) g_threadMain->SetRunning(false);  // self-referential pointer to flag the thread is running 
      boinc_finish(g_iQCNReturn); // this never returns, note ERR_FINISHED sets this to 0 (normal end is a workunit timeout of 1 day)
    }
#endif
	
    if (g_threadMain) g_threadMain->SetRunning(false);  // self-referential pointer to flag the thread is running 
    return g_iQCNReturn;
}

void checkContinualUpload(bool bForce)
{
   if (!g_bContinual) return; // quit if not continual upload!
    // check for more than 5 files in g_strPathContinual and if so upload then delete them
        // now make sure the zip file is stored in sm->strPathTrigger + ti.strFile
        int iSlot = (int) sm->iNumUpload;
        ZipFileList zfl;
        long lCurTime = QCN_ROUND(dtime() + qcn_main::g_dTimeOffset);
        char strPattern[_MAX_PATH], strResolve[_MAX_PATH], strLogical[_MAX_PATH_LOGICAL];
        memset(strPattern, 0x00, _MAX_PATH);
        memset(strResolve, 0x00, _MAX_PATH);
        memset(strLogical, 0x00, _MAX_PATH_LOGICAL);
        string strPath((const char*) qcn_main::g_strPathContinual);  // note it DOES NOT HAVE appropriate \ or / at the end
        int iRetVal = 0;

        iSlot++;  // increment the upload slot counter
        if (iSlot<1 || iSlot>MAX_UPLOAD) {  // sanity check, only zip file #'s 1 through MAX_UPLOAD
          fprintf(stderr, "%ld - No zip slots left (%d) for upload file!\n", lCurTime, iSlot);
          return;
        }

        // the pattern to upload is this workunit name & .zip  (in case there are old files leftover from other workunits etc)
        sprintf(strPattern, "%s|.zip", (const char*) sm->dataBOINC.wu_name);
        // just get all the files in the trigger dir 
        boinc_filelist(strPath, strPattern, &zfl);
        if (zfl.size() == 0 || (zfl.size() < 5 && !bForce)) {
           // not enough files to bother with uploading
           return;
        }

        // OK now we can resolve the zip filename
        sprintf(strLogical, "qcnout%d.zip", iSlot);
        if (boinc_resolve_filename(strLogical, strResolve, _MAX_PATH) && !strResolve[0]) {
          // this zip name didn't resolve, free sz mem and return!
          fprintf(stderr, "%ld - Upload zip filename %s not resolved for continual upload file!\n", lCurTime, strLogical);
          return;
        }

        // OK, zip up the files in zfl into the resolved upload filename
        boinc_begin_critical_section();
        if ((iRetVal = boinc_zip(ZIP_IT, strResolve, &zfl))) {
          fprintf(stderr, "%ld - Zip error %d for continual upload file %s!\n", lCurTime, iRetVal, strResolve);
          boinc_end_critical_section();
          return;
        }
        if (!boinc_file_exists(strResolve)) { // send this file, whether it was just made or made previously
          fprintf(stderr, "%ld - Continual upload file %s not found!\n", lCurTime, strResolve);
          boinc_end_critical_section();
          return;
        }

        // OK, process as usual
        sm->iNumUpload = iSlot;  // set the num upload which was successfully incremented & processed above
        // note boinc_upload_file (intermediate uploads) requires the logical boinc filename ("soft link")!
        qcn_util::sendIntermediateUpload(strLogical, strResolve);  // the logical name gets resolved by boinc_upload_file into full path zip file 

        // delete original files
	    ZipFileList::iterator izfl = zfl.begin();
		while (izfl != zfl.end()) {
           boinc_delete_file(izfl->c_str());  // don't need the original zip, since it's in the strResolve zip name
		   izfl++;
        }
        boinc_end_critical_section();

}

bool CheckTriggerFile(struct STriggerInfo& ti, bool bForce)
{  
    if (!ti.lOffsetEnd) return true;   // we're not in a trigger, just return true so this element gets deleted
	
	const int g_TenSecCount = (int) (10.0/sm->dt);  // reference number of points for 10 seconds

    long n1 = 0L, n2 = 0L, n2orig = 0L, lSM = sm->lOffset - 1; // note save current offset less 1, as could change during this function!

    // enum e_trigger { TRIGGER_UNSET, TRIGGER_IMMEDIATE, TRIGGER_10SEC, TRIGGER_20SEC, TRIGGER_30SEC, TRIGGER_1MIN, TRIGGER_2MIN, TRIGGER_DEMO };

    // default n1 is a minute ago
    n1 = ti.lOffsetEnd - (6 * g_TenSecCount); // one minute ago

    switch(ti.iLevel)
    { // set the window based on our trigger level
       case TRIGGER_DEMO:     // read-only mode write out file every 10 minutes
          n1 = ti.lOffsetStart;  // start
          n2 = ti.lOffsetEnd;  // start
          break;
       case TRIGGER_IMMEDIATE:    // immediately write out past minute and sm->lTriggerFile (or lOffset if greater)
          n1 = lSM - (6 * g_TenSecCount);  // one minute ago
          n2 = lSM;  // this is the latest point we have
          break;
/*
		case TRIGGER_10SEC:
          n2 = ti.lOffsetEnd + g_TenSecCount;
          break;
       case TRIGGER_20SEC:
          n2 = ti.lOffsetEnd + (2 * g_TenSecCount);
          break;
       case TRIGGER_30SEC:
          n2 = ti.lOffsetEnd + (3 * g_TenSecCount);
          break;
*/
       case TRIGGER_1MIN:
          n2 = ti.lOffsetEnd + (6 * g_TenSecCount);
          break;
       case TRIGGER_2MIN:
          n2 = ti.lOffsetEnd + (12 * g_TenSecCount);
          break;
       default: 
          return true; // delete this trigger, has an invalid level
    }

    //  STARTING POINT OF FILE -- one minute before trigger
    if (n1 < 0) { //wrapping around from beginning
      if (QCN_ROUND(sm->t0[n1 + MAXI])==0L) {
         n1 = 1;
      }
      else {
           n1 += MAXI;
      }
    }

    if (bForce) { // write out as much as possible, i.e. our current lOffset (-1 for safety);
       n2 = lSM;  // note will never be higher than MAXI so no adjustment/wrap-around checking necessary
    }
    else {
       //  ENDING POINT OF FILE -- two minutes after trigger
       if (n2 > MAXI) { // wrapping around at end
         n2orig = n2;
         n2 -= MAXI;
       }
    }
 
    // exit if we are in a trigger but not past our n2 write time -- note the wrapping at the end of the array though!
    // if not wrapping (n2orig = 0) and n2 is greater than our lOffset/lSM then we have not exceeded the write point
    // if wrapping (n2orig > 0) and current point is greater than our trigger offset, OR current point less than our n2, we have not exceeded the write point
    if (ti.bReal && !bForce && ( (!n2orig && lSM < n2) || (n2orig && (lSM > ti.lOffsetEnd || lSM < n2)))  ) {
         return false; 
    }

    // we reach here if past our window for this file
    // check if trigger was made in interactive mode, if so bypass file creation
    // but always allow demo-mode triggers to get written (i.e. 10-minute dumps)
    if (!ti.bInteractive || ti.bReal) {
       char strTypeSensor[8];
       memset(strTypeSensor, 0x00, sizeof(char) * 8);
       if (g_psms) strncpy(strTypeSensor, g_psms->getTypeStrShort(), sizeof(strTypeSensor));
       sacio::sacio(n1, n2, ti, strTypeSensor); // note filename already set in ti.strFile
    }

    // don't forget to reset level if it's > TRIGGER_2MIN
    // note we're faking that this trigger is all done, since demo mode we just write out every minute and TRIGGER_DEMO is > TRIGGER_ALL
    ti.iLevel++; // bump up trigger level

    return (bool) (ti.iLevel > TRIGGER_2MIN);  // return true means it will delete this iterator
}

// send a trickle when a trigger is hit, if bFollowUp==true it's a followup trigger with supplemental info
bool CheckTriggerTrickle(struct STriggerInfo& ti)
{
	// note - check for a followup trig required, if trig was already sent, and we're 5 seconds after it was sent, with no bFollowUp
	bool bFollowUp = ti.bSent && !ti.bSentFollowUp && (ti.lOffsetEnd + ceil(4.0 / sm->dt)) < sm->lOffset;
    if (!bFollowUp && (!ti.lOffsetEnd || ti.bSent || !ti.bReal || ti.bInteractive)) {
       return true;  // if no offset and/or already sent or a demo mode trickle (i.e. per-minute trigger) or interactive mode, can just return
    }

#ifdef QCNLIVE
	if (bFollowUp) return true;  //no need for followup triggers in qcnlive
#endif
	
    boinc_begin_critical_section();
    // mark this trigger as sent
    sm->setTriggerLock();
    sm->releaseTriggerLock();
 
    char *strTrigger = new char[1024];
    memset(strTrigger, 0x00, sizeof(char) * 1024);
	
    char *strFollowUp = new char[512];
    memset(strFollowUp, 0x00, sizeof(char) * 512);

    double dTriggerTime = sm->t0[ti.lOffsetEnd] + g_dTimeOffset; // this is the adjusted time of the trigger, i.e. should match server time

    // get xy & z components to send for the past 1 second i.e. to get max before trigger
    float fmax_xy[4], fmax_z[4];
	memset(fmax_xy, 0x00, sizeof(float) * 4);
	memset(fmax_z, 0x00, sizeof(float) * 4);

	qcn_util::get_fmax_components(ti.lOffsetEnd, fmax_xy, fmax_z, bFollowUp);
	ti.bSent = true;
	ti.bSentFollowUp = bFollowUp;

   
	// Trigger field tags:
    //    result_name = unique boinc work name [BOINC Scheduler adds]
    //    time   = boinc server database time received [BOIN CScheduler adds]
    //    vr     = QCN app version number i.e. 1.43
    //    sms    = sms type, check the e_sensor enum in define.h
    //    ctime   = the participant machine "date -u -p %f" time of the trigger -- adjusted by server sync time if bTimeOffset (<tsync>Y</tsync)
    //    fsig   = the significance value of this trigger
    //    fmag   = the magnitude of this trigger
    //    file   = the trigger SAC file for this trigger (will be zip'd), can be sent later (requested later)
    //    tsync  = Y means this host machine (therefore trigger) was successfully sync'd to the server so the time values are reliable (N=failed ntpdate lookup)
    //    reset  = number of timing error resets for this workunit, if any (higher number means problematic/unreliable machine) 
    //    dt     = the target "delta time" between points, usually .02 sec but could be bumped up to .1 on slow/unreliable machines
    //    loc    = unused now, will perhaps be an array of lat/lngs associated with this host machine (i.e. to track laptop movements etc)
    //    ipaddr = server side ip address [BOINC Scheduler adds]
	
	// followup vals: 
	//   mxy1p = max xy comp 1 sec prev
	//   mz1p = max z comp 1 sec prev
	//   mxy1a = max xy comp 1 sec after
	//   mz1a = max z comp 1 sec after
	//   mxy2a = max xy comp 2 sec after
	//   mz2a = max z comp 2 sec after
	//   mxy4a = max xy comp 4 sec after
	//   mz4a = max z comp 4 sec after
	if (bFollowUp) {  // do all vals above
		sprintf(strFollowUp, 
				"<follow>1</follow>\n"
				"<mxy1p>%f</mxy1p>\n"
				"<mz1p>%f</mz1p>\n"
				"<mxy1a>%f</mxy1a>\n"
				"<mz1a>%f</mz1a>\n"
				"<mxy2a>%f</mxy2a>\n"
				"<mz2a>%f</mz2a>\n"
				"<mxy4a>%f</mxy4a>\n"
				"<mz4a>%f</mz4a>\n",
				fmax_xy[0], fmax_z[0],
				fmax_xy[1], fmax_z[1],
				fmax_xy[2], fmax_z[2],
				fmax_xy[3], fmax_z[3]
				);
	}
	else  {  // just do the 1 sec prev values
		sprintf(strFollowUp, 
				"<mxy1p>%f</mxy1p>\n"
				"<mz1p>%f</mz1p>\n",
				fmax_xy[0], fmax_z[0]
				);
	}

    sprintf(strTrigger,
           "<vr>%s</vr>\n"
           "<os>%s</os>\n"
           "<sms>%d</sms>\n"
           "<ctime>%f</ctime>\n"
           "<fsig>%f</fsig>\n"
           "<fmag>%f</fmag>\n"
           "<file>%s</file>\n"
           "<reset>%d</reset>\n"
           "<dt>%f</dt>\n"
           "%s"
           "<tsync>%f</tsync>\n"
           "<toff>%f</toff>\n"
           "<%s>%.2f</%s>\n"
           "<%s>%.2f</%s>\n"
           "<qlat>%f</qlat>\n"
           "<qlng>%f</qlng>\n",
       QCN_VERSION_STRING,
       qcn_util::os_type_str(),
       sm->eSensor,
       dTriggerTime,
       sm->fsig[ti.lOffsetEnd],
       sm->fmag[ti.lOffsetEnd],
       ti.strFile,
       sm->iNumReset,
       sm->dt,
       strFollowUp,
       g_dTimeSync>0.0f ? g_dTimeSync + g_dTimeOffset : 0.0f,  // note we're sending the local client offset sync time adjusted to server time!
       g_dTimeOffset, 
          XML_CLOCK_TIME, sm->clock_time, XML_CLOCK_TIME,
          XML_CPU_TIME, sm->cpu_time, XML_CPU_TIME,
            sm->dTrLatitude, sm->dTrLongitude
    );

    trickleup::qcnTrickleUp(strTrigger, ti.iVariety, (const char*) sm->dataBOINC.wu_name);  // send a trigger for this trickle

    // filename already set in ti.strFile
	if (!bFollowUp) { // just print out a line for the original trigger
		fprintf(stdout, "%f  Trigger detected at offset %ld  time %f  write at %ld - zip file %s\n", 
			g_dTimeCurrent, ti.lOffsetEnd, sm->t0[ti.lOffsetEnd], (long) sm->itm, ti.strFile);
		fflush(stdout); 
	}

    delete [] strTrigger;
	delete [] strFollowUp;
    boinc_end_critical_section();
    return true;
}

bool CheckTriggers(bool bForce)
{
#ifdef _DEBUG
			static int iCount = -1;
			if (iCount != (int) g_vectTrigger.size()) {
				iCount = (int) g_vectTrigger.size();
			    fprintf(stdout, "DEBUG:  %d Outstanding Triggers\n", (int) g_vectTrigger.size());
			}
#endif
        int iRemove = 0;
        if (!g_vectTrigger.empty()) { // there are triggers to monitor, go through the vector
			// use an iterator
		   vector<struct STriggerInfo>::iterator ist = g_vectTrigger.begin();
			while (ist != g_vectTrigger.end() ) {
	         if (ist->lOffsetEnd) { // there's a non-zero lOffset, so better check for trickle or file I/O
                //if (!bForce) CheckTriggerTrickle(&ti); // note too late for a trigger if bForce is on, but maybe not for file I/O below
				CheckTriggerTrickle(*ist); // maybe not too late for a trigger if bForce is on
                if (CheckTriggerFile(*ist, bForce)) {  // check that it's time for trigger file I/O after this trickledd
                    // erase this one if returns true, i.e. we're done all I/O
					ist->bRemove = true;
					sm->setTriggerLock();
					ist = g_vectTrigger.erase(ist); 
					sm->releaseTriggerLock();					
					iRemove++;
#ifdef _DEBUG
					fprintf(stdout, "DEBUG:  Removed Old Trigger, %d remain\n", (int) g_vectTrigger.size());
#endif
                }
				else { // increment iterator
					ist++;
				}
             }
             else { // lOffset is 0 i.e. non-existent since can never trigger on 0 (initial baseline point)
                // if there's no lOffset, we can just get rid of this element
                // this should never happen if CheckTrigger* logic is working!
                 ist->bRemove = true;
				 sm->setTriggerLock();
				 ist = g_vectTrigger.erase(ist); 
				 sm->releaseTriggerLock();
				 iRemove++;
#ifdef _DEBUG
				 fprintf(stdout, "DEBUG:  Removed Old Trigger, %d remain\n", (int) g_vectTrigger.size());
#endif
			 }
           }
		}
        return true;
}

void parseArgs(int argc, char* argv[])
{
   // set perturb defaults which may be overridden on cmd-line
    g_fPerturb[PERTURB_SIG_CUTOFF] = DEFAULT_SIG_CUTOFF;
    g_fPerturb[PERTURB_SHORT_TERM_AVG_MAG] = DEFAULT_SHORT_TERM_AVG_MAG;
  
    // workunit name _fs??_ the ?? is sig cutoff, _stam??_ is short-term-avg mag 

    // parse command line arguments
    for (int i=0; i<argc; i++) { 
      /*
        if (sm && !strcmp(argv[i], "--dump") && (i+1)<argc && (argv[i+1])) 
        {
            strcpy((char*)sm->strCurFile, argv[i+1]);
            if (sm->deserialize(sm, sizeof(CQCNShMem), (const char*) sm->strCurFile)) {
              sm->bReadOnly = true; // flag it's in read-only mode due to the deserial
              fprintf(stdout, "QCN memory read from file %s\n", sm->strCurFile);
              fprintf(stdout, "Running in demo mode; no live sensor data used, no output.\n");
              fflush(stdout);
            }
        } 

        if (sm && !strcmp(argv[i], "--sigcutoff")) { // override the default sigcutoff
          // if this is present, the next arg is the sig cutoff filter value
          if ((i+1)<argc && argv[i+1]) {
             g_fPerturb[PERTURB_SIG_CUTOFF] = (float) atof(argv[i+1]);
          }
        }

        if (sm && !strcmp(argv[i], "--demo"))
        {   // run in demo mode
            g_bDemo = true; // flag it's in demo mode
            fprintf(stdout, "QCN running in interactive mode - time sync to server, but no trigger trickles to server.\n");
                      fprintf(stdout, "All SAC file output in sac/ subdirectory\n");
            fflush(stdout);
        }
        */

        // Important -- continual workunits set here as a cmd line argument, or check if exec is qcncontinual
        if (!g_bContinual && !strcmp(argv[i], "--continual"))
        {   // run in continual mode
            g_bContinual = true; // flag it's in demo mode
            fprintf(stdout, "QCN running in continual output mode:\n - time sync to server, upload trigger files hourly to QCN server.\n");
            fflush(stdout);
        }
    }
}

} // namespace qcn_main
