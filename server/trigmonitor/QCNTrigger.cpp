#include <iostream>
#include <cassert>
#include <iomanip>

#include "QCNTrigger.h"

#ifdef ONLINE
#include "../trigger/qcn_types.h"
#endif

using namespace std;


//int QCNTrigger::NSHORT = 255
//Constructors
QCNTrigger::QCNTrigger() :
        qcn_quakeid(0),
        posted(false),
        hostid(-1),
        triggerid(-1),
        longitude(0.0),
        latitude(0.0),
        time_trigger(0.0),
        time_received(0.0),
        time_est(0.0),
        significance(0.0),
        magnitude(0.0),
        c_cnt(0),
        c_ind(vector<int> (N_SHORT,-9)),
        c_hid(vector<int> (N_SHORT,-99)),
        dis(0.0),
        pors(0),
        dirty(false)
{
    init();
}

//Destructor
QCNTrigger::~QCNTrigger()
{}

#ifdef ONLINE
/////////////////////////////////PUBLIC METHODS::///////////////////////////////////////////
void
QCNTrigger::copyFromUSBTrigger( const DB_QCN_TRIGGER_MEMORY& qtm)
{
    // Only use triggers from usb accelerometers
    this->hostid    = qtm.hostid;              // Host ID
    this->triggerid = qtm.triggerid;        // Trigger ID
    this->qcn_quakeid= qtm.qcn_quakeid;     // bring in the matching quake id, if any yet!
    this->posted = qtm.posted;               // file req posted?
    strncpy(this->db, qtm.db_name, sizeof(this->db)-1);   // Database Name
    strncpy(this->file, qtm.file, sizeof(this->file)-1);  // File name
    strncpy(this->result_name, qtm.result_name, sizeof(this->result_name)-1);  // File name
    this->latitude = qtm.latitude;           // Latitude
    this->longitude = qtm.longitude;          // Longitude
    this->time_trigger = qtm.time_trigger;   // Trigger Time
    this->time_received = qtm.time_received; // Time Trigger received
    this->significance  = qtm.significance;  // Significance (Trigger detection filter)
    this->magnitude  = qtm.magnitude;        // set mag to magnitude at time of trigger
    //ask this why just 4 seconds, whey 1 sec resolution, why not such as 10 secs
    this->pgah[0] = qtm.mxy1p;               // Peak Ground Acceleration (1 second before and during trigger) (m/s/s)
    this->pgaz[0] = qtm.mz1p;                // Peak Ground Acceleration (1 seconds before and during trigger) (m/s/s)
    this->pgah[1] = qtm.mxy1a;               // Peak Ground Acceleration (1 second after trigger)  (m/s/s)
    this->pgaz[1] = qtm.mz1a;                // Peak Ground Acceleration (1 second after trigger)  (m/s/s)
    this->pgah[2] = qtm.mxy2a;               // Peak Ground Acceleration (2 seconds after trigger)  (m/s/s)
    this->pgaz[2] = qtm.mz2a;                // Peak Ground Acceleration (2 seconds after trigger)  (m/s/s)
    this->pgah[3] = qtm.mxy4a;               // Peak Ground Acceleration (4 seconds after trigger) (m/s/s)
    this->pgaz[3] = qtm.mz4a;                // Peak Ground Acceleration (4 seconds after trigger) (m/s/s)

    for (int j=0;j<4;j++) {
        if (this->pgah[j] > this->magnitude)
            this->magnitude = this->pgah[j];
        if (this->pgaz[j] > this->magnitude)
            this->magnitude = this->pgaz[j];
    }



}
#endif

void
QCNTrigger::setMagnitude()
{
    for (int j=0;j<4;j++) {
        if (this->pgah[j] > this->magnitude)
            this->magnitude = this->pgah[j];
        if (this->pgaz[j] > this->magnitude)
            this->magnitude = this->pgaz[j];
    }

}

void
QCNTrigger::print()
{

    cout << "long          = " << longitude     << endl;
    cout << "lat           = " << latitude      << endl;
    cout << "mag           = " << magnitude     << endl;
    cout << "hostid        = " << hostid        << endl;
    cout << "triggerid     = " << triggerid     << endl;
    cout << "file          = " << file          << endl;
    cout << "time_trigger  = " << setprecision(14) << time_trigger  << endl;
    cout << "time_received = " << setprecision(14) << time_received << endl;
    cout << "significance  = " << significance  << endl;
    cout << "dis           = " << dis           << endl; 
    cout << "pgah       pgaz " <<  endl;
    for(int i=0; i<4; i++) {
        cout << "   " << pgah[i]  << "     " << pgaz[i] << endl;
    }
    
    cout << " c_cnt = " << c_cnt << endl;
    cout << " c_ind    chost:" << endl;
    for (int i = 0; i < c_cnt; i++ ){
        cout << "   " << c_ind[i] << "    " << c_hid[i] << endl;
    }
 
}


////////////////PRIVATE METHODS//////////////////////////////
void
QCNTrigger::init()
{
//    for(int i=0; i<N_SHORT; i++) {
//        c_ind[i]  = -9;
//        c_hid [i] = -1;
//   }

    for(int i=0; i<4; i++) {
        pgah[i] = 0.0;
        pgaz[i] = 0.0;
    }
    //clear();
}
