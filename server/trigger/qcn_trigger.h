/*

CMC -- this is included with the boinc/sched/handle_request.C -- basically it will override the "message from host" insert (mfh.insert) to send triggers
mfh.variety == "trigger" to a separate table for speed & ease of lookups

*/

#ifndef _QCN_TRIGGER_H
#define _QCN_TRIGGER_H

#include "config.h"
#include <cassert>
#include <cstdio>
#include <vector>
#include <string>
#include <ctime>
#include <cmath>
using namespace std;

#include <unistd.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/stat.h>

#ifdef _USING_FCGI_
#include "boinc_fcgi.h"
#endif

#include "boinc_db.h"
#include "backend_lib.h"
#include "error_numbers.h"
#include "parse.h"
#include "str_util.h"
#include "str_replace.h"
#include "util.h"
#include "filesys.h"
#include "sched_types.h"
#include "sched_util.h"
#include "sched_msgs.h"

#include "qcn_types.h"

// the curl executable to use for Maxmind GeoIP queries (would linking to curl lib be better?  but have to tell BOINC)
//#define EXEC_CURL     "/usr/local/bin/curl"
#define FORMAT_MAXMIND "https://geoip.maxmind.com/b?l=ILFoClxbJcfk&i=%s.1"

#define BYTESIZE_URL   64
#define BYTESIZE_CURL 512

#ifndef _MAX_PATH
#define _MAX_PATH 255
#endif

extern DB_CONN boinc_db;
#ifdef USE_QCN_TRIGGER_MEMORY_TABLE
  extern DB_CONN trigmem_db;
#endif

extern int qcn_doTriggerHostLookup(
   DB_QCN_HOST_IPADDR& qhip,
   DB_QCN_GEO_IPADDR&  qgip,
   DB_QCN_TRIGGER&     qtrig,
   const double* dmxy,
   const double* dmz
);

#endif  // ifndef _QCN_TRIGGER_H
