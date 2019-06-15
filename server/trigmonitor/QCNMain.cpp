#include <iostream>
#include <typeinfo>
#include <fstream>
#include <sstream>
#include <string>



//CXXFLAGS = -ftemplate-depth-30 -g -DBZ_DEBUG  for DEBUG MODE
#include "qcn_types.h"
#include "QCNTrigger.h"
#include "QCNEvent.h"
#include "QCNBounds.h"
#include "Crust2.h"
#include "QCN.h"

#include "sched_msgs.h"
#include "sched_config.h"


using namespace std;

int main(int argc, char **argv)
{
    // initialize random seed:
    srand ( time(NULL) );

    /* CMC check paths
    fprintf(stdout, "%s\n", EVENT_PATH);
    fprintf(stdout, "%s\n", BAD_HOSTS_FILE);
    fprintf(stdout, "%s\n", EMAIL_DIR);
    fprintf(stdout, "%s\n", EMAIL_INC);
    fprintf(stdout, "%s\n", EMAIL_PATH);
    fprintf(stdout, "%s\n", GMT_MAP_PHP);
    */
    //Crust2 object
    Crust2  crust2;

    // load CRUST2.0 3D seismic velocity model for the crust:
    int retval = -1;
    if ((retval = crust2.load())) {
        log_messages.printf(MSG_CRITICAL,
                            "Can't load crust2 data %d\n", retval
                           );
        return -1;
    }

    //SCHED_CONFIG config is defined /home/boinc/projects/boinc/sched/sched_config.cpp;
    retval = config.parse_file();
    if (retval) {
        log_messages.printf(MSG_CRITICAL,
                            "Can't parse config.xml: %s\n", boincerror(retval)
                           );
        return 1;
    }


    double dSleepInterval = -1.0;
    int    iTriggerTimeInterval = -1;
    int    iTriggerDeleteInterval = -1;
    for (int i=1; i<argc; i++) {
        if (!strcmp(argv[i], "-d")) {
            log_messages.set_debug_level(atoi(argv[++i]));
        } else if (!strcmp(argv[i], "-sleep_interval")) {
            dSleepInterval = atof(argv[++i]);
        } else if (!strcmp(argv[i], "-time_interval")) {
            iTriggerTimeInterval = atoi(argv[++i]);
        } else if (!strcmp(argv[i], "-delete_interval")) {
            iTriggerDeleteInterval = atoi(argv[++i]);
        } else {
            log_messages.printf(MSG_CRITICAL,
                                "bad cmdline arg: %s\n\n"
                                "Example usage: trigmon -d 3 -sleep_interval 3 -time_interval 10\n\n", argv[i]
                               );
            return 2;
        }
    }

    log_messages.printf(MSG_NORMAL,
                        "trigmon started with the following options:\n"
                        "  -time_interval   = %d\n"
                        "  -sleep_interval  = %f\n",
                        iTriggerTimeInterval,
                        dSleepInterval
                       );

    //construct QCN object
    QCN qcn(crust2);
    qcn.setSleepInterval(dSleepInterval); 
    qcn.setTriggerTimeInterval(iTriggerTimeInterval);
    qcn.setTriggerDeleteInterval(iTriggerDeleteInterval);
    qcn.execute();

    return 0;

}

