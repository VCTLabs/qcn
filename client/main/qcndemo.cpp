// CMC -- this will run the demo packages

#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
#include "qcn_config_win.h"
#else
#include "config.h"
#endif
#include "define.h"
#include "checkboincstatus.h"
#include "qcn_gzip.h"
#include "qcn_util.h"
#include "qcn_shmem.h"
#include "qcn_thread.h"
#include "qcn_signal.h"
#include "boinc_zip.h"

CQCNShMem* volatile sm = NULL;
char strSuffix[128];
#ifdef _WIN32
#include <conio.h>
const char strApp[] = {"qcndemo.exe"};
#else
const char strApp[] = {"qcndemo"};
#endif
const char strGraphics[] = {"qcn_graphics"};
const int iLenReply = 1024;  // that should be plenty for the return value, it's probably more like 100 chars across two lines
char strReply[1024];
char strCWD[_MAX_PATH];
ZipFileList zfl;

#ifdef _WIN32
#define popen _popen
#define pclose _pclose
DWORD WINAPI QCNThreadGraphics(LPVOID);
#else
extern void* QCNThreadGraphics(void*);
#endif

int runit(const char* strCmd)
{
   int iRetVal = 0;
   FILE* f = popen(strCmd, "r");
   if (f) {
      pclose(f);
      iRetVal = 1;
   }
   return iRetVal;
}

void LaunchGraphicsThread();

e_endian check_endian(void) {
  int x = 1;
  if(*(char *)&x == 1)
    return ENDIAN_LITTLE;
  else
    return ENDIAN_BIG;
}

int main(int argc, char** argv)
{
    enum e_os ge_os;
    e_endian g_endian = check_endian(); // coped from util/qcn_util.cpp -- g_endian is global to all procs, i.e. the sac file I/O utils can now use it
    const char strMain[] = {"qcn"};
    memset(strReply, 0x00, iLenReply * sizeof(char));

    char strPath[_MAX_PATH];
    memset(strCWD, 0x00, _MAX_PATH * sizeof(char));
    
    ///Users/carlc/qcn/client/test/qcndemo
    char* strEnd = strrchr(argv[0], 'q');  // the first q found at the end should be our program
    if (strEnd) {
       strncpy(strCWD, argv[0], strEnd - argv[0]);
#ifdef _WIN32
       chdir(strCWD);
#else
       _chdir(strCWD);
#endif
	}
    fprintf(stdout, "Current directory is [%s]\n", strCWD);

#ifdef __APPLE_CC__
    if (g_endian == ENDIAN_BIG) { // powerpc
        ge_os = OS_MAC_PPC;
        sprintf(strSuffix, "_%s_powerpc-apple-darwin9.7.0", QCN_VERSION_STRING);
    }
    else {
        ge_os = OS_MAC_INTEL;
        sprintf(strSuffix, "_%s_i686-apple-darwin9.7.0", QCN_VERSION_STRING);
    }    
#else
#ifdef _WIN32
        ge_os = OS_WINDOWS;
#ifdef _DEBUG
        sprintf(strSuffix, "d.exe");
#else
        sprintf(strSuffix, "_%s_%s.exe", QCN_VERSION_STRING, BOINC_WIN_SUFFIX);
#endif
#else // Linux
        ge_os = OS_LINUX;
        sprintf(strSuffix, "_%s_i686-pc-linux-gnu", QCN_VERSION_STRING);
#endif
#endif
   

    int bStatus;
    fprintf(stdout, "Getting latest earthquake list...");
    fflush(stdout);
#ifdef _WIN32
	bStatus = runit("projects\\qcn.edu_qcn\\curl.exe -s http://quakecatcher.net/qcnalpha/download/qcn-quake.xml > slots/0/qcn-quake.xml");
#else
	bStatus = runit("curl -s http://quakecatcher.net/qcnalpha/download/qcn-quake.xml > slots/0/qcn-quake.xml");
#endif
    if (!bStatus)  {
#ifdef _WIN32
       bStatus = runit("type slots\\0\\init.1 slots\\0\\qcn-quake.xml slots\\0\\init.2 > slots\\0\\init_data.xml");
#else
       bStatus = runit( "cat slots/0/init.1 slots/0/qcn-quake.xml slots/0/init.2 > slots/0/init_data.xml");
#endif
    }
    else  {
       fprintf(stdout, "Error:\n%s\n", strReply);
       fflush(stdout);
    }

    // do any OS specific stuff, i.e. copy over ntpdate
    switch (ge_os) {
       case OS_WINDOWS:
            runit( "copy slots\\0\\ntpdate.win32 slots\\0\\ntpdate");
            break;
       case OS_MAC_PPC:
            runit( "cp slots/0/ntpdate.mac-ppc slots/0/ntpdate");
            break;
       case OS_MAC_INTEL:
            runit( "cp slots/0/ntpdate.mac-intel slots/0/ntpdate");
            break;
       default:
            break;
    }

#ifdef _WIN32
       runit("del /q /s /f projects\\qcn.edu_qcn\\triggers");
       runit("del /q /s /f slots\\0\\qcnprefs.xml slots\\0\\stderr.txt slots\\0\\boinc_quake_0 slots\\0\\trick*.xml");
#ifdef _WIN32
       chdir("slots\\0");
#else
       _chdir("slots\\0");
#endif
	   sprintf(strPath, "%s%s%s", "..\\..\\projects\\qcn.edu_qcn\\", strMain, strSuffix);
#else
       runit("rm -rf projects/qcn.edu_qcn/triggers");
       runit("rm -f slots/0/qcnprefs.xml slots/0/stderr.txt slots/0/boinc_quake_0 slots/0/trick*.xml");
       _chdir("slots/0");
       sprintf(strPath, "%s%s%s", "../../projects/qcn.edu_qcn/", strMain, strSuffix);
#endif

       LaunchGraphicsThread();

       fprintf(stdout, "Launching %s...\n", strPath);
       fprintf(stdout, "SAC file output in sac/\n");
       fprintf(stdout, "QCN Sensor output in qcndemo.txt\n");
       fprintf(stdout, "Press Ctrl+C to Quit\n");
       fflush(stdout);

       char strExec[_MAX_PATH];
#ifdef _WIN32
       sprintf(strExec, "%s --demo>..\\..\\qcndemo.txt", strPath);
#else
       sprintf(strExec, "%s --demo>../../qcndemo.txt", strPath);
#endif
       runit(strExec);

}


void LaunchGraphicsThread()
{
    CQCNThread threadGraphics(QCNThreadGraphics);
    threadGraphics.Start();
}


#ifdef _WIN32
DWORD WINAPI QCNThreadGraphics(LPVOID)
#else
extern void* QCNThreadGraphics(void*)
#endif
{ 
    // this just calls the ntpdate executable and then gets & sets the sm->dOffset time stuff
    char strGraphicsExec[_MAX_PATH];
#ifdef _WIN32
    sprintf(strGraphicsExec, "%s%s%s", "..\\..\\projects\\qcn.edu_qcn\\", strGraphics, strSuffix);
#else
    sprintf(strGraphicsExec, "%s%s%s", "../../projects/qcn.edu_qcn/", strGraphics, strSuffix);
#endif
	fprintf(stdout, "Launching %s...\n", strGraphicsExec);
    fflush(stdout);

    system(strGraphicsExec);
    return NULL;
}

