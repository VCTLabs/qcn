// CMC Note -- this file is actually included from boinc/sched/handle_request.C so as to make it easier
//             to add the necessary functions and code

// CMC -- now we need to handle triggers differently and put them into the appropriate table
//     at the point this function is called we know this is a trigger trickle (i.e. variety='trigger')

#include "qcn_trigger.h"
#include <curl/curl.h>
#include "sched_config.h"

   /*
      Note: all references to "IP address" are really the first 3 bytes of an ipaddress, i.e. the 127.0.0 part of 127.0.0.1
      This is done to save lookups on the GeoIP database (which costs money)

      At this point we are given the trickle (trigger) message as a const ptr to DB_MSG_FROM_HOST class
      from this class we would be interested in the create_time, hostid, and xml fields; particular the
      xml field which should be parsed into the various DB/mysql structs we would need for insertion into the
      qcn_trigger table

      However before insertion much logic needs to be done to get a nominal lat/long for this trigger:

         1) We need to match the IP address in pmfh->xml<extip> with a value in the qcn_host_ipaddr table.
            Note that we need an outer join on IP addr to do this, if lat/lng null then....
           a) If this IP addr is found in qcn_host_ipaddr, get the lat/lng from there for insertion into qcn_trigger.
               1) Note there could be more than one IP->lat/lng mapping, i.e. it could have been a user inputted IP->lat/lng map
                   as well as a GEO IP lookup -- so sort results in order of geoipaddrid (so the user added one, geoipaddrid=0 will be 
                   the top one)
               2) Note we are always "favoring" a user-inputted lat/lng mapping
   
            b) if no IP addr found for this host in the qcn_host_ipaddr table, look up in the qcn_geo_ipaddr table
               1) if found (and record isn't older than 6 months say), then we can use this record (i.e. no need to do another GeoIP lookup, saving a penny ;-)
               2) if not found, we'll need to do a GeoIP web service lookup and insert the record into both the qcn_geo_ipaddr and
                  the qcn_host_ipaddr tables (with a pointer qcn_host_ipaddr.geoipaddr to the qcn_geo_ipaddr.id record)

         2) At this point we should have a "user-guided" or GeoIP mapping between the incoming IP address and their lat/long location, so
            we can insert the trigger.  

         3) Triggers will probably be displayed on the user's host page and perhaps edit lat/long after an event to update qcn_host_ipaddr?

      Here is an example of the data in pmfh->xml :

<result_name>qcne_000105_0</result_name>
      <time>1201209164</time>
<vr>2.20</vr>
<sms>1</sms>
<ctime>1201209161.669771</ctime>
<fsig>3.929932</fsig>
<fmag>7.179575</fmag>
<file>qcne_000105_000110_1201209161.zip</file>
<reset>1</reset>
<dt>0.020000</dt>
<loc></loc>
<tsync>1201208379.748502</tsync>
<toff>-2.146548</toff>
<wct>234.34</wct>
<cpt>4.5632</cpt>
<extip>171.64.173.42</extip>

        This should be parsed out to give us matching fields in qcn_host_ipaddr:

 id             int(11)      NO    PRI                  
 hostid         int(11)      YES   MUL  NULL            
 ipaddr         varchar(32)  YES        NULL            
 result_name    varchar(64)  NO
 time_trigger   double       YES   MUL  NULL            
 time_received  double       YES        NULL            
 time_sync      double       YES        NULL            
 sync_offset    double       YES        NULL            
 significance   double       YES        NULL            
 magnitude      double       YES        NULL            
 latitude       double       YES        NULL            
 longitude      double       YES        NULL            
 levelvalue     float        YES        NULL            
 levelid        smallint     YES        NULL            
 alignid 
 file           varchar(64)  YES        NULL            
 dt             float        YES        NULL            
 numreset       int(6)       YES        NULL            
 qcn_sensorid    int(2)       YES        NULL            
 sw_version     varchar(8)
 os_type        varchar(8)
 qcn_quakeid   int(11)
 received_file  bool

  (note we store result_name and not resultid to save time on the lookup, qcn_quakeid can also be used later to link a known event with our trigger)

   */

long g_curlBytes = 0L;

int qcn_process_ipaddr(char* strIPAddr, int iLen);

// forward declaration for the big function at the bottom of this file
int lookupGeoIPWebService(
   const int iRetGeo,
   DB_QCN_HOST_IPADDR& qhip,
   DB_QCN_GEO_IPADDR&  qgip,
   DB_QCN_TRIGGER&     qtrig,
   const double* const dmxy,
   const double* const dmz
);

// decl for curl function
bool execute_curl(const char* strURL, char* strReply, const int iLen);

// decl for curl write function
size_t qcn_curl_write_data(void *ptr, size_t size, size_t nmemb, void *stream);

// if we have a qcn_trigger_memory table for live monitoring of events - insert the trigger
#ifdef USE_QCN_TRIGGER_MEMORY_TABLE

bool doTriggerMemoryUpdate(const DB_QCN_TRIGGER& qtrig, const double* dmxy, const double* dmz)
{
  // don't put in triggers into memory which haven't had a time sync as they can be way off
  // also we just want varietyid=0 (i.e. normal triggers), also skip out if no valid file name
  if (qtrig.varietyid !=0 || qtrig.time_sync < 1e6 || strlen(qtrig.file) < 5) return false;

  char* strFields = new char[512];
  memset(strFields, 0x00, 512);
  sprintf(strFields, "mxy1p=%f, mz1p=%f, mxy1a=%f, mz1a=%f, mxy2a=%f, mz2a=%f, mxy4a=%f, mz4a=%f", 
       dmxy[0], dmz[0],
       dmxy[1], dmz[1],
       dmxy[2], dmz[2],
       dmxy[3], dmz[3]
   );

  char* strQuery = new char[1024];
  memset(strQuery, 0x00, 1024);

  sprintf(strQuery, "UPDATE trigmem.qcn_trigger_memory SET %s WHERE file='%s'", strFields, qtrig.file);

  DB_QCN_TRIGGER_MEMORY qtrigmem;
  int retval = qtrigmem.db->do_query(strQuery);

  if (retval) {
       log_messages.printf(
           SCHED_MSG_LOG::MSG_CRITICAL,
           "[QCN] [HOST#%d] [RESULTNAME=%s] [TIME=%lf] Error - could not update followup qcn_trigger_memory info for %s\n",
            qtrig.hostid, qtrig.result_name, qtrig.time_received,
            qtrig.file
          );
  }
  else {
       log_messages.printf(
           SCHED_MSG_LOG::MSG_DEBUG,
           "[QCN] [HOST#%d] [RESULTNAME=%s] [TIME=%lf] Successfully updated trigmem followup qcn_trigger_memory info for %s\n",
            qtrig.hostid, qtrig.result_name, qtrig.time_received,
            qtrig.file
          );
  }
  
  // now try the sensor/continual qcn_trigger table update
  sprintf(strQuery, "UPDATE qcn_trigger SET %s WHERE file='%s'", strFields, qtrig.file);
  retval = qtrig.db->do_query(strQuery);
  if (retval) {
       log_messages.printf(
           SCHED_MSG_LOG::MSG_CRITICAL,
           "[QCN] [HOST#%d] [RESULTNAME=%s] [TIME=%lf] Error - could not update followup qcn_trigger info for %s\n",
            qtrig.hostid, qtrig.result_name, qtrig.time_received,
            qtrig.file
          );
  }
  else {
       log_messages.printf(
           SCHED_MSG_LOG::MSG_DEBUG,
           "[QCN] [HOST#%d] [RESULTNAME=%s] [TIME=%lf] Successfully updated trigmem followup qcn_trigger info for %s\n",
            qtrig.hostid, qtrig.result_name, qtrig.time_received,
            qtrig.file
          );
  }

  delete [] strFields;
  delete [] strQuery;

  return true;
}

bool doTriggerMemoryInsert(const DB_QCN_TRIGGER& qtrig, const double* const dmxy, const double* const dmz)
{  // call this after inserting a "regular" trigger record - this will add the 
   // trigger (if applicable i.e. insertid>0, timesync>0) to the memory table for event polling

    // don't put in triggers into memory which haven't had a time sync as they can be way off
    // also we just want varietyid=0 (i.e. normal triggers)
    // also don't bother putting into memory if trigger time was over 60 seconds from received time
    if (qtrig.varietyid !=0 
      || qtrig.time_sync < 1e6
      || qtrig.time_trigger + 60.0 < qtrig.time_received) 
         return false;

    int iVal = qtrig.db->insert_id();
    if (iVal <= 0) return false;  // invalid trigger ID

    DB_QCN_TRIGGER_MEMORY qtrigmem;

    strncpy(qtrigmem.db_name, config.db_name, 15);   // database name from config file is stored in mem table
    qtrigmem.triggerid = iVal;  // memory trigger ID matches disk table trigger ID
    qtrigmem.qcn_quakeid = 0;
    qtrigmem.posted = 0;

    // copy over remainig trigger fields of interest
    qtrigmem.hostid = qtrig.hostid;
    strncpy(qtrigmem.ipaddr, qtrig.ipaddr, 31);
    strncpy(qtrigmem.result_name, qtrig.result_name, 63);
    strncpy(qtrigmem.file, qtrig.file, 63);
    qtrigmem.time_trigger = qtrig.time_trigger;
    qtrigmem.time_received = qtrig.time_received;
    qtrigmem.time_sync = qtrig.time_sync;
    qtrigmem.sync_offset = qtrig.sync_offset;
    qtrigmem.significance = qtrig.significance;
    qtrigmem.magnitude = qtrig.magnitude;
    qtrigmem.latitude = qtrig.latitude;
    qtrigmem.longitude = qtrig.longitude;
    qtrigmem.levelvalue = qtrig.levelvalue;
    qtrigmem.levelid = qtrig.levelid;
    qtrigmem.alignid = qtrig.alignid;
    qtrigmem.dt = qtrig.dt;
    qtrigmem.numreset = qtrig.numreset;
    qtrigmem.qcn_sensorid = qtrig.qcn_sensorid;
    qtrigmem.varietyid = qtrig.varietyid;
    qtrigmem.hostipaddrid = qtrig.hostipaddrid;
    qtrigmem.geoipaddrid = qtrig.geoipaddrid;

    //if (dmxy[0] > -DBL_MAX) qtrigmem.mxy1p = dmxy[0];
    //if (dmz[0] > -DBL_MAX) qtrigmem.mz1p = dmz[0];
    qtrigmem.mxy1p = dmxy[0];
    qtrigmem.mz1p = dmz[0];

    iVal = qtrigmem.insert();
    if (iVal) { //error
         log_messages.printf(
           SCHED_MSG_LOG::MSG_CRITICAL,
           "[QCN] [HOST#%d] [RESULTNAME=%s] [TIME=%lf] [0] Could not insert trigmem.qcn_trigger_memory record\n",
           qtrig.hostid, qtrig.result_name, qtrig.time_received
         );
    }
    return (iVal == 0);
}
#endif  // USE_QCN_TRIGGER_MEMORY_TABLE

// handle_qcn_trigger processes the trigger trickle, does the geoip or database lookup as appropriate, inserts into qcn_trigger
int handle_qcn_trigger(const DB_MSG_FROM_HOST* pmfh, const int iVariety, DB_QCN_HOST_IPADDR& qhip)
{
     // instantiate the objects
     DB_QCN_GEO_IPADDR  qgip;
     DB_QCN_TRIGGER     qtrig;
     
     char strIP[32]; // temp holder for IP address
     int iRetVal = 0;
     int iFollowUp = 0;
     bool bFollowUp = false;
     double dmxy[4], dmz[4];
     for (int i = 0; i < 4; i++) {
        dmxy[i] = 0.0;
        dmz[i] = 0.0;
     }

     // parse out all the data into the qtrig object;
       qtrig.mxy1p = 0.0;
       qtrig.mz1p = 0.0;
       qtrig.mxy1a = 0.0;
       qtrig.mz1a = 0.0;
       qtrig.mxy2a = 0.0;
       qtrig.mz2a = 0.0;
       qtrig.mxy4a = 0.0;
       qtrig.mz4a = 0.0;
     qtrig.hostid = pmfh->hostid; // don't parse hostid, it's in the msg_from_host struct!
     if (!parse_str(pmfh->xml, "<result_name>", qtrig.result_name, sizeof(qtrig.result_name))) memset(qtrig.result_name, 0x00, sizeof(qtrig.result_name));
     if (!parse_str(pmfh->xml, "<vr>", qtrig.sw_version, sizeof(qtrig.sw_version))) memset(qtrig.sw_version, 0x00, sizeof(qtrig.sw_version));
     if (!parse_str(pmfh->xml, "<os>", qtrig.os_type, sizeof(qtrig.os_type))) memset(qtrig.os_type, 0x00, sizeof(qtrig.os_type));
     parse_int(pmfh->xml, "<sms>", qtrig.qcn_sensorid);
     parse_int(pmfh->xml, "<reset>", qtrig.numreset);
     parse_double(pmfh->xml, "<dt>", qtrig.dt);
     parse_double(pmfh->xml, "<tsync>", qtrig.time_sync);
     parse_double(pmfh->xml, "<toff>", qtrig.sync_offset);
     parse_double(pmfh->xml, "<wct>", qtrig.runtime_clock);
     parse_double(pmfh->xml, "<cpt>", qtrig.runtime_cpu);
     if (!parse_str(pmfh->xml, "<extip>", strIP, 32)) memset(strIP, 0x00, sizeof(char) * 32);
     parse_double(pmfh->xml, "<ctime>", qtrig.time_trigger);
     if (isnan(qtrig.time_trigger)) qtrig.time_trigger= 0;
     // check for follow up info
     if (!parse_int(pmfh->xml, "<follow>", iFollowUp)) iFollowUp = 0;
     bFollowUp = (bool) (iFollowUp == 1);  // must be exactly 1 else error

     // check for followup info if any, i.e. 1 sec prev, 1 sec after 2 sec after 4 sec after data for xy component & z component
     // all normal triggers will possibly have the 1 sec prev values
     if (!parse_double(pmfh->xml, "<mxy1p>", dmxy[0])) dmxy[0] = 0;     
     if (!parse_double(pmfh->xml, "<mz1p>", dmz[0])) dmz[0] = 0;     

     if (bFollowUp) {  // only followup triggers will have the 4 sec after values
       if (!parse_double(pmfh->xml, "<mxy1a>", dmxy[1])) dmxy[1] = 0;
       if (!parse_double(pmfh->xml, "<mz1a>", dmz[1])) dmz[1] = 0;
  
       if (!parse_double(pmfh->xml, "<mxy2a>", dmxy[2])) dmxy[2] = 0;
       if (!parse_double(pmfh->xml, "<mz2a>", dmz[2])) dmz[2] = 0;

       if (!parse_double(pmfh->xml, "<mxy4a>", dmxy[3])) dmxy[3] = 0;
       if (!parse_double(pmfh->xml, "<mz4a>", dmz[3])) dmz[3] = 0;

       qtrig.mxy1p = dmxy[0];
       qtrig.mz1p = dmz[0];
       qtrig.mxy1a = dmxy[1];
       qtrig.mz1a = dmz[1];
       qtrig.mxy2a = dmxy[2];
       qtrig.mz2a = dmz[2];
       qtrig.mxy4a = dmxy[3];
       qtrig.mz4a = dmz[3];
     }

// CMC hack - change JW 7 to 100, MN 8 to 101
/*
     switch(qtrig.qcn_sensorid) {
        case 7:
           qtrig.qcn_sensorid = 100;
           break;
        case 8:
           qtrig.qcn_sensorid = 101;
           break;
      }
*/

     qtrig.flag = 0;
     qtrig.time_received = dtime();  // mark current server time as time_received, this gets overridden by database unix_timestamp() in qcn_trigger.h db_print

     if (!parse_str(pmfh->xml, "<file>", qtrig.file, sizeof(qtrig.file))) memset(qtrig.file, 0x00, sizeof(qtrig.file));
     if (iVariety) {
       if (qtrig.time_trigger < 1.0f) qtrig.time_trigger = qtrig.time_received; // sometimes trigtime/<ctime> is sent, older versions it wasn't
       qtrig.significance = 0;  // blank out sig/mag for not normal (variety!=0) triggers
       qtrig.magnitude = 0;
       if (iVariety != 2) strcpy(qtrig.file,"");   // no filename if not continual or normal trigger
       qtrig.varietyid = iVariety;
     }
     else {
       if (!parse_double(pmfh->xml, "<fsig>", qtrig.significance)) qtrig.significance = 0;;
       if (!parse_double(pmfh->xml, "<fmag>", qtrig.magnitude)) qtrig.magnitude = 0;
       qtrig.varietyid = 0;
       if (isnan(qtrig.significance)) qtrig.significance = 0;
       if (isnan(qtrig.magnitude)) qtrig.magnitude = 0;

       // fudge database hack - if normal trigger - check for continual trigger amongst the real trig if sw version < 5.47
       if ( atof(qtrig.sw_version) < 5.47f && strstr(qtrig.result_name, "continual_") ) {
          qtrig.significance = 0.0f;  // blank out sig/mag for not normal (variety!=0) triggers
          qtrig.magnitude = 0.0f;
          qtrig.varietyid = 2;
       }

     }


     /*
       // so at this point, for qtrig we lack:
       latitude       double       YES        NULL            
       longitude      double       YES        NULL            
       levelvalue     float        YES        NULL            
       levelid        smallint     YES        NULL            
       alignid
       ipaddr         varchar(32)
       received_file qcn_quakeid should be null
     */

     // eventid & qcn_quakeid will be used later, 
     // in case we want to tie in some USGS or QCN or other event id to this trigger
     qtrig.qcn_quakeid   = 0;
     qtrig.received_file = 0;
     qtrig.levelvalue = 0;
     qtrig.levelid    = 0;
     qtrig.alignid = 0;

     // so it's really latitude, longitude, levelvalue, levelid and ipaddr left to add to qcn_trigger object & database table

     // let's get the proper ipaddr form
     // don't forget we only "count" the first three bytes of an IP address, so process strIP for qtrig.ipaddr...
     qcn_process_ipaddr(strIP, 32);
     memset(qtrig.ipaddr, 0x00, 32);
     strncpy(qtrig.ipaddr, strIP, 31);

     // at this point, if a followup trigger, we can jsut update the memory table qcn_trigger_memory and split
     if (bFollowUp) {
       #ifdef USE_QCN_TRIGGER_MEMORY_TABLE
           doTriggerMemoryUpdate(qtrig, dmxy, dmz); // update the qcn_trigger table with followup data
       #endif
       return 0;
     }

     // OK, now just the lat/lng lookup
     // the first step will be to search into qcn_host_ipaddr to see if this exists already, sorted by geoip so user setting will be preferred
     
     qhip.hostid = qtrig.hostid; // important -- copy over ipaddr & hostid into other structs
     strcpy(qhip.ipaddr, qtrig.ipaddr);
     strcpy(qgip.ipaddr, qtrig.ipaddr);

     log_messages.printf(
           SCHED_MSG_LOG::MSG_DEBUG,
           "[QCN] [HOST#%d] [RESULTNAME=%s] [TIME=%lf] Processing QCN %s trickle message from IP %s\n",
           qtrig.hostid, qtrig.result_name, qtrig.time_received, 
              iVariety ? (iVariety==1 ? "ping" : "continual") : "trigger", qtrig.ipaddr
     );

     iRetVal = qcn_doTriggerHostLookup(qhip, qgip, qtrig, dmxy, dmz);

     // at this point we've inserted the appropriate records or had a database error, 
     // so return with iRetVal (0=good, otherwise a database error which will "nak" so the trigger will be resent)
     return iRetVal; // returns qhip.lookup error if there was a database error, so trigger won't be "ack'd" and will try again
}


int qcn_process_ipaddr(char* strIPAddr, int iLen)
{ // takes a full IP address (32 bits/8bytes) and converts it for QCN use 24 bits/3 bytes
     bool bIPOK = false;
     char* strLast = strrchr(strIPAddr, '.');  // this finds the last . in an ip address, so we want everything before that
     if (strLast > strIPAddr) { // we found the last bit, so let's copy over everything between to the qtrig.ipaddr structure
        strLast[0] = '\0';  // put a null terminator where the last . is -- simple & effective!
        bIPOK = true;
     }
     // now validate the IP address prefix in qtrig.ipaddr, if not in the form X.Y.Z then it's invalid
     // and should be set to null ("")
     strLast = strchr(strIPAddr, '.');  // this finds the first . in an ip address, so we want everything before that
     if (!strLast || strlen(strLast) <= 1 || !strchr(strLast+1, '.')) { // this finds the next ., if this is null, reset qtrig.ipaddr as it's a bad IP addr
         // can't be a good IP prefix since strlen too small and/or only 1 . found
         // can't be a good IP prefix since strlen too small and/or only 1 . found
         log_messages.printf(
           SCHED_MSG_LOG::MSG_CRITICAL,
           "[QCN] [0] Invalid IP address detected: %s\n",
           strIPAddr
         );
         memset(strIPAddr, 0x00, iLen);
         bIPOK = false;
     }
     return (bIPOK ? 0 : 1); //success
}

int qcn_doTriggerHostLookup(
   DB_QCN_HOST_IPADDR& qhip,
   DB_QCN_GEO_IPADDR&  qgip,
   DB_QCN_TRIGGER&     qtrig,
   const double* dmxy,
   const double* dmz
)
{

   char* strErr = NULL;
   char strWhere[_MAX_PATH];
   int iRetVal = 0;
 // CMC HERE - change to just grab the latest qcn_host_ipaddr record for this hostid if a USB sensor (i.e. static location)

     // note if IP address is '' then this just becomes the user pref lat/lng if they input a record with no IP address
     // also note the sort order, non-geoip lookups (i.e. user-entered lat/lng/ipaddr) get sorted to the top

     if (qtrig.qcn_sensorid >= 100) // usb sensor
       sprintf(strWhere, "WHERE hostid=%d ORDER BY geoipaddrid, id LIMIT 1", qtrig.hostid);
     else
       sprintf(strWhere, "WHERE hostid=%d AND (ipaddr='' OR ipaddr='%s') ORDER BY geoipaddrid,ipaddr", qtrig.hostid, qtrig.ipaddr);

     iRetVal = qhip.lookup(strWhere);
     switch(iRetVal) {
        case ERR_DB_NOT_FOUND:  // this host & IP addr not found (or else it's a geopip record), check for host record with no IP, then need to lookup in geoip
           // OK, if ipaddr is empty and record not found, we can't do much (i.e. no geoip lookup on an empty ip address!), so just return 0
           if (!strlen(qtrig.ipaddr)) {
              log_messages.printf(
                SCHED_MSG_LOG::MSG_CRITICAL,
                "[QCN] [HOST#%d] [RESULTNAME=%s] [TIME=%lf] [1] Invalid IP address and no host/lat/lng default record found\n",
                qtrig.hostid, qtrig.result_name, qtrig.time_received
              );
              return 0;
           }

           // no record, need to do a maxmind/geoip database table lookup, and possibly web service lookup!
           sprintf(strWhere, "WHERE ipaddr='%s'", qtrig.ipaddr);
           iRetVal = lookupGeoIPWebService(qgip.lookup(strWhere), qhip, qgip, qtrig, dmxy, dmz);

           /*

           // first off, let's search for a hostid record in qcn_host_ipaddr, WITHOUT an ipaddr
           // i.e. perhaps the user entered a lat/lng to always apply to this host?
           sprintf(strWhere, "WHERE hostid=%d AND ipaddr='' AND geoipaddrid=0", qtrig.hostid);
           iRetVal = qhip.lookup(strWhere);
           switch(iRetVal) {
              case ERR_DB_NOT_FOUND:  // no record, need to do a maxmind/geoip database table lookup, and possibly web service lookup!
                 sprintf(strWhere, "WHERE ipaddr='%s'", qtrig.ipaddr);
                 iRetVal = lookupGeoIPWebService(qgip.lookup(strWhere), qhip, qgip, qtrig); 
                 break;
              case 0:  // a qcn_host_ipaddr record found, insert into host table and set lat/lng for qcn_trigger
                 // copy over the lat/lng for this record into qtrig
                 memset(qtrig.ipaddr, 0x00, sizeof(qtrig.ipaddr));
                 qtrig.latitude  = qhip.latitude;
                 qtrig.longitude = qhip.longitude;
                 qtrig.levelvalue = qhip.levelvalue;
                 qtrig.levelid = qhip.levelid;
                 qtrig.alignid = qhip.alignid;
                 qtrig.hostipaddrid = qhip.id;
                 qtrig.geoipaddrid = qhip.geoipaddrid;
                 iRetVal = qtrig.insert();  // note if the insert fails, return code will be set and returned below
                 if (!iRetVal) { // trigger got in OK
                    #ifdef USE_QCN_TRIGGER_MEMORY_TABLE
                       doTriggerMemoryInsert(qtrig, dmxy, dmz);
                    #endif
                    log_messages.printf(
                          SCHED_MSG_LOG::MSG_DEBUG,
                          "[QCN] [HOST#%d] [RESULTNAME=%s] [TIME=%lf] [1] Trigger inserted after qcn_host_ipaddr lookup of blank IP, mag=%lf at (%lf, %lf)!\n",
                          qtrig.hostid, qtrig.result_name, qtrig.time_received, qtrig.magnitude, qtrig.latitude, qtrig.longitude
                    );
                 }
                 break;
              default:  // other database error, will be returned below and trigger will be input later
                 log_messages.printf(
                    SCHED_MSG_LOG::MSG_CRITICAL,
                    "[QCN] [HOST#%d] [RESULTNAME=%s] [TIME=%lf] [1] Database error encountered on qcn_host_ipaddr lookup!\n",
                    qtrig.hostid, qtrig.result_name, qtrig.time_received 
                 );
           }
           */
           break;
                   
        case 0:   // a qcn_host_ipaddr record found for this host & ipaddr 
           // (either a user entry with or without ipaddr key,  or previous geoip/maxmind lookup for this ipaddr)
           // copy over the lat/lng for this record into qtrig
           qtrig.latitude  = qhip.latitude;
           qtrig.longitude = qhip.longitude;
           qtrig.levelvalue = qhip.levelvalue;
           qtrig.levelid = qhip.levelid;
           qtrig.alignid = qhip.alignid;
           qtrig.hostipaddrid = qhip.id;
           qtrig.geoipaddrid = qhip.geoipaddrid;
           iRetVal = qtrig.insert();  // note if the insert fails, return code will be set and returned below, for update later
           if (iRetVal) {
              strErr = new char[512];
              memset(strErr, 0x00, 512);
              qtrig.db_print(strErr);
              log_messages.printf(
                SCHED_MSG_LOG::MSG_CRITICAL, "qhip/trigger insert errcode %d - %s\n", iRetVal, strErr);
              delete [] strErr;  strErr = NULL;
           }
           else { // trigger got in OK
                #ifdef USE_QCN_TRIGGER_MEMORY_TABLE
                   doTriggerMemoryInsert(qtrig, dmxy, dmz);
                #endif
                log_messages.printf(
                  SCHED_MSG_LOG::MSG_DEBUG,
                  "[QCN] [HOST#%d] [RESULTNAME=%s] [TIME=%lf] [1] Trigger inserted after qcn_host_ipaddr lookup of IP %s, mag=%lf at (%lf, %lf) - sync offset %f at %f!\n",
                  qtrig.hostid, qtrig.result_name, qtrig.time_received, qtrig.ipaddr, qtrig.magnitude, qtrig.latitude, qtrig.longitude, qtrig.sync_offset, qtrig.time_sync
                );
           }
           break;
        default:   // other database error, so return error
           log_messages.printf(
            SCHED_MSG_LOG::MSG_CRITICAL,
            "[QCN] [HOST#%d] [RESULTNAME=%s] [TIME=%lf] [9] Database error encountered on trigger processing!\n",
            qtrig.hostid, qtrig.result_name, qtrig.time_received
           );
     }
   return iRetVal;
}

int lookupGeoIPWebService(
   const int iRetGeo,
   DB_QCN_HOST_IPADDR& qhip,
   DB_QCN_GEO_IPADDR&  qgip,
   DB_QCN_TRIGGER&     qtrig,
   const double* dmxy,
   const double* dmz
)
{
                 int iReturn = iRetGeo; // "seed" our return code with the initial return code value
                 char *strURL = NULL, *strReply = NULL;

                 // the switch is the reply/return code from the lookup query into the qcn_geo_ipaddr table
                 // note that we may want to set error return code (iRetGeo) to 0 if it seems that this record
                 // will never get input, otherwise it will "nak" and retry (but if it's a bad GeoIP lookup, why bother?)

                 switch(iRetGeo) {
                    case ERR_DB_NOT_FOUND:  // no record, need to do a maxmind/geoip web service lookup!
#ifndef _USE_MAXMIND
                       break;
#endif
                       strURL   = new char[BYTESIZE_URL];
                       strReply = new char[BYTESIZE_CURL];
                       memset(strURL, 0x00, sizeof(char) * BYTESIZE_URL);
                       memset(strReply, 0x00, sizeof(char) * BYTESIZE_CURL);
                       sprintf(strURL, FORMAT_MAXMIND, qtrig.ipaddr);
                       if (strlen(qtrig.ipaddr) > 6 && strlen(qtrig.ipaddr) < 16
                         && execute_curl(strURL, strReply, BYTESIZE_CURL))  {
                          // returned OK, now check strReply -- should be a single line of comma-delimited fields:
                          // Returns: ISO 3166 Two-letter Country Code, Region Code, City, Latitude, Longitude, Error code
                          // good reply: (note 4 commas/5 fields)
                          //    GB,K2,Oxford,51.750000,-1.250000
                          // error reply: (note 5 commas/6 fields all null except last)
                          //    ,,,,,IP_NOT_FOUND
                          char* strComma[6] = {NULL, NULL, NULL, NULL, NULL, NULL};
                          int i = 0;
                          strComma[0] = strchr(strReply, ',');  // get the first comma
                          for (i = 1; strComma[0] && i < 6; i++)  { // parse out fields
                             // search for next comma if last value wasn't NULL and haven't gone past the end of strReply
                             if (strComma[i-1] && strlen(strReply) > (size_t)(strComma[i-1] - strReply + 1)) {
                                strComma[i] = strchr(strComma[i-1]+1, ','); // note we skip a char to move off current comma ptr
                                if (!strComma[i]) break;  // if this is null, i.e.no comma found, may as well break
                             }
                          }
                          if (i<4 || strComma[4] || strComma[5]) { // if this isn't null, or less than 4 commas found, then there was an error
                            iReturn = 0; // ip not found, but this is a bad format web service lookup, so let's not bother retrying...
                            log_messages.printf(
                              SCHED_MSG_LOG::MSG_CRITICAL,
                              "[QCN] [HOST#%d] [RESULTNAME=%s] [TIME=%lf] [2] Maxmind/GeoIP web lookup for IP %s via %s to %s failed\nReply: %s\n",
                              qtrig.hostid, qtrig.result_name, qtrig.time_received, qtrig.ipaddr, "libcurl", strURL, strReply
                            );
                          }
                          else {
                            // seems like a legit reply, so parse out
                            // note we need to insert into qcn_geo_ipaddr, get insert_id(), then insert into host_ipaddr & trigger
                            iReturn = 0; // success

                            // initialize vars for the copies
                            memset(&qgip.country, 0x00, sizeof(qgip.country));
                            memset(&qgip.region, 0x00, sizeof(qgip.region));
                            memset(&qgip.city, 0x00, sizeof(qgip.city));

                            // set location as geoip
                            strcpy(qhip.location, "geoip");    // mark as geoip (also noted in field  geoipaddr i.e points to qcn_geo_ipaddr record)

                            qgip.time_lookup = dtime(); // returns a double of current time, # of seconds since epoch

                            strncpy(qgip.country, strReply, strComma[0]-strReply);  // first comma entry is country; note length is OK starting from strReply
                            strncpy(qgip.region, strComma[0]+1, strComma[1]-strComma[0]-1);  // next is region, note subtract 1 from length (, position)
                            strncpy(qgip.city, strComma[1]+1, strComma[2]-strComma[1]-1);  // next is city

                            char *strTmp = new char[32];
                            memset(strTmp, 0x00, sizeof(char) * 32);
                            strncpy(strTmp, strComma[2]+1, strComma[3]-strComma[2]-1);  // next is latitude
                            qgip.latitude = safe_atof(strTmp);

                            memset(strTmp, 0x00, sizeof(char) * 32);
                            strComma[4] = strReply + strlen(strReply);  // make a fake endpoint
                            strncpy(strTmp, strComma[3]+1, strComma[4]-strComma[3]-1);  // next is longitude
                            qgip.longitude = safe_atof(strTmp);

                            delete [] strTmp;

                            iReturn = qgip.insert();
                            if (!iReturn) { // success, get insert_id
                               int iInsertID = qgip.db->insert_id();
                               if (iInsertID>0) {
                                    // now make a host record
                                    qhip.geoipaddrid = iInsertID;  // mark the geoip database id used 
                                    qtrig.geoipaddrid = qhip.geoipaddrid;
                                    qtrig.latitude   = qgip.latitude;
                                    qtrig.longitude  = qgip.longitude;
                                    qhip.latitude    = qgip.latitude;
                                    qhip.longitude   = qgip.longitude;
                                    qhip.levelvalue = 0;
                                    qhip.levelid    = 0;
                                    qhip.alignid    = 0;
                                    qtrig.levelvalue = 0;
                                    qtrig.levelid    = 0;
                                    qtrig.alignid    = 0;
                                    iReturn = qhip.insert();
                                    if (!iReturn) { // success, insert trigger, if fails retcode sent below
                                        qtrig.hostipaddrid = qhip.db->insert_id(); // need the qcn_host_ipaddr id for trigger table
                                        iReturn = qtrig.insert();
                                        if (iReturn) {
                                            log_messages.printf(
                                              SCHED_MSG_LOG::MSG_CRITICAL,
                                              "[QCN] [HOST#%d] [RESULTNAME=%s] [TIME=%lf] [2] Maxmind/GeoIP web lookup -- trigger %s insert failed\n",
                                              qtrig.hostid, qtrig.result_name, qtrig.time_received, qtrig.ipaddr
                                            );
                                        }
                                        else {
                                            #ifdef USE_QCN_TRIGGER_MEMORY_TABLE
                                               doTriggerMemoryInsert(qtrig, dmxy, dmz);
                                            #endif
                                            log_messages.printf(
                                              SCHED_MSG_LOG::MSG_DEBUG,
                                              "[QCN] [HOST#%d] [RESULTNAME=%s] [TIME=%lf] [2] Maxmind/GeoIP web lookup -- trigger %s insert success\n",
                                              qtrig.hostid, qtrig.result_name, qtrig.time_received, qtrig.ipaddr
                                            );
                                        }
                                    }
                                    else {
                                       iReturn = 0; // well we tried, return 0 so doesn't bother with this trigger again
                                       log_messages.printf(
                                            SCHED_MSG_LOG::MSG_CRITICAL,
                                            "[QCN] [HOST#%d] [RESULTNAME=%s] [TIME=%lf] [2] Maxmind/GeoIP web lookup -- host_ipaddr %s insert failed\n",
                                            qtrig.hostid, qtrig.result_name, qtrig.time_received, qhip.ipaddr
                                       );
                                    } // failed qhip insert
                               } // insertid
                               else { // failed qgip insert id
                                  log_messages.printf(
                                    SCHED_MSG_LOG::MSG_CRITICAL,
                                    "[QCN] [HOST#%d] [RESULTNAME=%s] [TIME=%lf] [2] Maxmind/GeoIP web lookup -- invalid insert id on geo_ipaddr %s\n",
                                    qtrig.hostid, qtrig.result_name, qtrig.time_received, qhip.ipaddr
                                  );
                               }
                            }
                            else { // bad geo_ipaddr insert
                                  log_messages.printf(
                                    SCHED_MSG_LOG::MSG_CRITICAL,
                                    "[QCN] [HOST#%d] [RESULTNAME=%s] [TIME=%lf] [2] Maxmind/GeoIP web lookup -- geo_ipaddr %s insert failed\n",
                                    qtrig.hostid, qtrig.result_name, qtrig.time_received, qhip.ipaddr
                                  );
                            }
                          }
                       }
                       else { // error in curl execution, set iReturn to non-zero so it can try again, should we insert trigger anyway?
                          iReturn = 2;
                          log_messages.printf(
                             SCHED_MSG_LOG::MSG_CRITICAL,
                             "[QCN] [HOST#%d] [RESULTNAME=%s] [TIME=%lf] [2a] Maxmind/GeoIP web lookup of IP %s via %s to %s failed\n",
                             qtrig.hostid, qtrig.result_name, qtrig.time_received, qtrig.ipaddr, "libcurl", strURL
                          );
                       }
                       break;
                    case 0:  // record found already in geoip table, insert into host table and set lat/lng for qcn_trigger
                       qhip.geoipaddrid = qgip.id;  // mark the geoip database id used 
                       qhip.latitude    = qgip.latitude;
                       qhip.longitude   = qgip.longitude;
                       qtrig.latitude   = qgip.latitude;
                       qtrig.longitude  = qgip.longitude;
                       qhip.levelvalue = 0;
                       qhip.levelid    = 0;
                       qhip.alignid    = 0;
                       qtrig.levelvalue = 0;
                       qtrig.levelid    = 0;
                       qtrig.alignid    = 0;

                       iReturn = qhip.insert();  // note if the insert fails, return code will be set and returned below
                       qtrig.geoipaddrid = qhip.geoipaddrid;
                       if (!iReturn) { // success, insert trigger, if fails retcode sent below
                           qtrig.hostipaddrid = qhip.db->insert_id();
                           iReturn = qtrig.insert();  // note if the insert fails, return code will be set and returned below
                       }
                       if (iReturn) { // error, print out debug info
              char* strErr = new char[512];
              memset(strErr, 0x00, 512);
              qtrig.db_print(strErr);
              log_messages.printf(
                SCHED_MSG_LOG::MSG_CRITICAL, "geoip lookup trigger insert\nerrcode %d - %s\n", iReturn, strErr);
              memset(strErr, 0x00, 512);
              qhip.db_print(strErr);
              log_messages.printf(
                SCHED_MSG_LOG::MSG_CRITICAL, "qhip trigger insert\nerrcode %d - %s\n", iReturn, strErr);
              delete [] strErr;  strErr = NULL;
                       }
                       else {
                          #ifdef USE_QCN_TRIGGER_MEMORY_TABLE
                              doTriggerMemoryInsert(qtrig, dmxy, dmz);
                          #endif
                          // trigger got in OK
                           log_messages.printf(
                             SCHED_MSG_LOG::MSG_DEBUG,
                             "[QCN] [HOST#%d] [RESULTNAME=%s] [TIME=%lf] [3] Trigger inserted after qcn_geo_ipaddr lookup; mag=%lf at (%lf, %lf) - sync offset %f at %f!\n",
                             qtrig.hostid, qtrig.result_name, qtrig.time_received, qtrig.magnitude, qtrig.latitude, qtrig.longitude, qtrig.sync_offset, qtrig.time_sync
                           );
                       }
                       break;
                    default:  // other database error, iReturn will be returned below
                       log_messages.printf(
                          SCHED_MSG_LOG::MSG_CRITICAL,
                          "[QCN] [HOST#%d] [RESULTNAME=%s] [TIME=%lf] [3] - Database error encountered on qcn_geo_ipaddr lookup!\n",
                          qtrig.hostid, qtrig.result_name, qtrig.time_received
                       );
                 }
                 if (strURL) delete [] strURL; // don't forget to get rid of these dynamic strings!
                 if (strReply) delete [] strReply;

                 return iReturn;
}

bool execute_curl(const char* strURL, char* strReply, const int iLen)
{
  memset(strReply, 0x00, iLen);
#ifndef USE_MAXMIND
  strncpy(strReply, "GB,K2,Oxford,51.750000,-1.25000", iLen-1);
  return true;
#endif

   // easycurl should be fine, just send a request to maxmind (strURL has the key & ip etc),
   // and output to strReply up to iLen size
   CURLcode cc;
   CURL* curlHandle = curl_easy_init();

   g_curlBytes = 0L;  // reset long num of curl bytes read

   if (!curlHandle) return false;  // problem with init

   cc = curl_easy_setopt(curlHandle, CURLOPT_VERBOSE, 0L);
   cc = curl_easy_setopt(curlHandle, CURLOPT_NOPROGRESS, 1L);
   cc = curl_easy_setopt(curlHandle, CURLOPT_URL, strURL);
   cc = curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, strReply);
   cc = curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, qcn_curl_write_data);

   cc = curl_easy_perform(curlHandle);

   curl_easy_cleanup(curlHandle);

   return (bool) (cc == 0 && sizeof(strReply)>0);  // 0 is good CURLcode
}

size_t qcn_curl_write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
   int iLeft = BYTESIZE_CURL - g_curlBytes - 1;
   if (iLeft > 0 && size > 0) { // we have some room left to write
      strlcat((char*) stream, (char*) ptr, BYTESIZE_CURL);
   }
   g_curlBytes += (size * nmemb);
   return size * nmemb;
}


