/*

CMC -- this is included with the boinc/sched/handle_request.C -- basically it will override the "message from host" insert (mfh.insert) to send triggers
mfh.variety == "trigger" to a separate table for speed & ease of lookups

*/

#ifndef _QCN_TYPES_H
#define _QCN_TYPES_H

#include "config.h"
#include <cassert>
#include <cstdio>
#include <vector>
#include <string>
#include <ctime>
#include <cmath>
using namespace std;

#ifdef _USING_FCGI_
#include "boinc_fcgi.h"
#endif

#include "boinc_db.h"
#include "parse.h"
#include "str_util.h"
#include "str_replace.h"
#include "util.h"
#include "filesys.h"

extern DB_CONN boinc_db;
#ifdef USE_QCN_TRIGGER_MEMORY_TABLE
  extern DB_CONN trigmem_db;
#endif

#define ESCAPE(x) escape_string(x, sizeof(x))
#define UNESCAPE(x) unescape_string(x, sizeof(x))


// structures matching the mySQL tables for QCN defined by qcn/server/qcn-data.sql

struct QCN_HOST_IPADDR 
{
    int id;
    int hostid;
    char ipaddr[32];
    char location[32];
    double latitude;
    double longitude;
    float levelvalue;
    int levelid;
    int alignid;
    int geoipaddrid;
    void clear() {memset(this, 0x00, sizeof(QCN_HOST_IPADDR));}
};

struct QCN_GEO_IPADDR 
{
    int id;
    char ipaddr[32];
    double time_lookup;
    char country[32];
    char region[32];
    char city[32];
    double latitude;
    double longitude;
    void clear() {memset(this, 0x00, sizeof(QCN_GEO_IPADDR));}
};

struct QCN_TRIGGER 
{
    int id;
    int hostid;
    char ipaddr[32];
    char result_name[64];
    double time_trigger;
    double time_received;
    double time_sync;
    double sync_offset;
    double significance;
    double magnitude;
    double mxy1p;
    double mz1p;
    double mxy1a;
    double mz1a;
    double mxy2a;
    double mz2a;
    double mxy4a;
    double mz4a;
    double latitude;
    double longitude;
    float  levelvalue;
    int levelid;
    int alignid;
    char file[64];
    double dt;
    int numreset;
    int qcn_sensorid;
    char sw_version[9];
    char os_type[9];
    int qcn_quakeid;
    double time_filereq;
    int received_file;
    double runtime_clock;
    double runtime_cpu;
    int varietyid;
    int flag;
    int hostipaddrid;
    int geoipaddrid;
    void clear() {memset(this, 0x00, sizeof(QCN_TRIGGER)); varietyid = -1; }
};

#ifdef USE_QCN_TRIGGER_MEMORY_TABLE
struct QCN_TRIGGER_MEMORY
{
    char db_name[16];
    int triggerid;
    int hostid;
    char ipaddr[32];
    char result_name[64];
    double time_trigger;
    double time_received;
    double time_sync;
    double sync_offset;
    double significance;
    double magnitude;
    double mxy1p;
    double mz1p;
    double mxy1a;
    double mz1a;
    double mxy2a;
    double mz2a;
    double mxy4a;
    double mz4a;
    double latitude;
    double longitude;
    float  levelvalue;
    int levelid;
    int alignid;
    char file[64];
    double dt;
    int numreset;
    int qcn_sensorid;
    int varietyid;
    int qcn_quakeid;
    int hostipaddrid;
    int geoipaddrid;
    bool posted;
    void clear() 
     {
       memset(this, 0x00, sizeof(QCN_TRIGGER_MEMORY));
       mxy1p = 0;
       mz1p = 0;
       mxy1a = 0;
       mz1a = 0;
       mxy2a = 0;
       mz2a = 0;
       mxy4a = 0;
       mz4a = 0;
     }
};
#endif

struct QCN_QUAKE
{
   int id;
   double time_utc;
   double magnitude;
   double depth_km;
   double latitude;
   double longitude;
   char description[256];
   int processed;
   char url[256];
   char guid[256];
   void clear() {memset(this, 0x00, sizeof(QCN_QUAKE));}
};

class DB_QCN_HOST_IPADDR : public DB_BASE, public QCN_HOST_IPADDR 
{
public:
    DB_QCN_HOST_IPADDR(DB_CONN* dc=0) :
          DB_BASE("qcn_host_ipaddr", dc ? dc : &boinc_db)  { clear(); }

    DB_ID_TYPE get_id() {return id;}

    void db_print(char* buf)
    { 
      char strLevelValue[32], strLevelID[16];
      if (levelid==0) {
        strcpy(strLevelValue, "NULL");
        strcpy(strLevelID, "NULL");
      }
      else {
        sprintf(strLevelValue, "%f", levelvalue);
        sprintf(strLevelID, "%d", levelid);
      }
      sprintf(buf,
        "hostid=%d,"
        "ipaddr='%s',"
        "location='%s',"
        "latitude=%16.9f,"
        "longitude=%16.9f,"
        "levelvalue=%s,"
        "levelid=%s,"
        "alignid=%d,"
        "geoipaddrid=%d",
        hostid, ipaddr, location, latitude, longitude, 
          strLevelValue, strLevelID, alignid,
        geoipaddrid
      );
    }

    void db_parse(MYSQL_ROW& r)
    {
      int i=0;
      clear();
      id = safe_atoi(r[i++]);
      hostid = safe_atoi(r[i++]);
      strcpy2(ipaddr, r[i++]);
      strcpy2(location, r[i++]);
      latitude = safe_atof(r[i++]);
      longitude = safe_atof(r[i++]);
      levelvalue = safe_atof(r[i++]);
      levelid = safe_atoi(r[i++]);
      alignid = safe_atoi(r[i++]);
      geoipaddrid = safe_atoi(r[i++]);
    }
};

extern int handle_qcn_trigger(const DB_MSG_FROM_HOST* pmfh, const int iVariety, DB_QCN_HOST_IPADDR& qhip);

// we need to store multiple entries of host-IP address lookups per host,
// as well as some info on number of GeoIP entries vs user-entered entries
// this is because people are putting in IP addresses or single entries
/*
class qcn_host_ipaddr_multiple
{
   public:
     int m_iNumGeoIP; // the number of geoip entries, usually at least 1
     int m_iNumEntered; // the number of user IP entries entered, usually from 0 to 5
     vector vHostIP<DB_QCN_HOST_IPADDR>;
};
*/

class DB_QCN_GEO_IPADDR : public DB_BASE, public QCN_GEO_IPADDR 
{
public:
    DB_QCN_GEO_IPADDR(DB_CONN* dc=0) :
          DB_BASE("qcn_geo_ipaddr", dc ? dc : &boinc_db)  { clear(); }

    DB_ID_TYPE get_id() {return id;}

    void db_print(char* buf)
    {
      ESCAPE(city);
      ESCAPE(country);
      ESCAPE(region);
      sprintf(buf,
        "ipaddr='%s',"
        "time_lookup=%f,"
        "country='%s',"
        "region='%s',"
        "city='%s',"
        "latitude=%16.9f,"
        "longitude=%16.9f",
        ipaddr, time_lookup, country, region, city, latitude, longitude
      );
      UNESCAPE(city);
      UNESCAPE(country);
      UNESCAPE(region);
    }

    void db_parse(MYSQL_ROW& r)
    {
      int i=0;
      clear();
      id = safe_atoi(r[i++]);
      strcpy2(ipaddr, r[i++]);
      time_lookup = safe_atof(r[i++]);
      strcpy2(country, r[i++]);
      strcpy2(region, r[i++]);
      strcpy2(city, r[i++]);
      latitude = safe_atof(r[i++]);
      longitude = safe_atof(r[i++]);
    }
};

class DB_QCN_TRIGGER : public DB_BASE, public QCN_TRIGGER 
{
public:
    DB_QCN_TRIGGER(DB_CONN* dc=0) :
          DB_BASE("qcn_trigger", dc ? dc : &boinc_db)  { clear(); }

    DB_ID_TYPE get_id() {return id;}

    void db_print(char* buf)
    {
      char strLevelValue[32], strLevelID[16];
      if (levelid==0) {
        strcpy(strLevelValue, "NULL");
        strcpy(strLevelID, "NULL");
      }
      else {
        sprintf(strLevelValue, "%f", levelvalue);
        sprintf(strLevelID, "%d", levelid);
      }
      sprintf(buf,
        "hostid=%d,"
        "ipaddr='%s',"
        "result_name='%s',"
        "time_trigger=%f,"
        "time_received=unix_timestamp(),"
        "time_sync=%f,"
        "sync_offset=%f,"
        "significance=%f,"
        "magnitude=%f,"
        "mxy1p=%f,"
        "mz1p=%f,"
        "mxy1a=%f,"
        "mz1a=%f,"
        "mxy2a=%f,"
        "mz2a=%f,"
        "mxy4a=%f,"
        "mz4a=%f,"
        "latitude=%16.9f,"
        "longitude=%16.9f,"
        "levelvalue=%s,"
        "levelid=%s,"
        "alignid=%d,"
        "file='%s',"
        "dt=%f,"
        "numreset=%d,"
        "qcn_sensorid=%d,"
        "sw_version='%s',"
        "os_type='%s',"
        "qcn_quakeid=%d,"
        "time_filereq=%f,"
        "received_file=%d,"
        "runtime_clock=%f,"
        "runtime_cpu=%f,"
        "varietyid=%d,"
        "flag=0,"
        "hostipaddrid=%d,"
        "geoipaddrid=%d"
        ,
        hostid, ipaddr, result_name, time_trigger, time_sync, sync_offset,
        significance, magnitude, 
        mxy1p,
        mz1p,
        mxy1a,
        mz1a,
        mxy2a,
        mz2a,
        mxy4a,
        mz4a,
        latitude, longitude, strLevelValue, strLevelID, alignid,
        file, dt, numreset, qcn_sensorid, sw_version, os_type,
        qcn_quakeid, time_filereq, received_file, runtime_clock, runtime_cpu, varietyid, 
        hostipaddrid, geoipaddrid
      );
    }

    void db_parse(MYSQL_ROW& r)
    {
      int i=0;
      clear();
      id = safe_atoi(r[i++]);
      hostid = safe_atoi(r[i++]);
      strcpy2(ipaddr, r[i++]);
      strcpy2(result_name, r[i++]);
      time_trigger = safe_atof(r[i++]);
      time_received = safe_atof(r[i++]);
      time_sync = safe_atof(r[i++]);
      sync_offset = safe_atof(r[i++]);
      significance = safe_atof(r[i++]);
      if (isnan(significance)) significance = 0;
      magnitude = safe_atof(r[i++]);
      if (isnan(magnitude)) magnitude = 0;  // some reason sig &  or mag can be NaN
      mxy1p = safe_atof(r[i++]);
      mz1p = safe_atof(r[i++]);
      mxy1a = safe_atof(r[i++]);
      mz1a = safe_atof(r[i++]);
      mxy2a = safe_atof(r[i++]);
      mz2a = safe_atof(r[i++]);
      mxy4a = safe_atof(r[i++]);
      mz4a = safe_atof(r[i++]);
      if (isnan(mxy1p)) mxy1p = 0;
      if (isnan(mxy1a)) mxy1a = 0;
      if (isnan(mxy2a)) mxy2a = 0;
      if (isnan(mxy4a)) mxy4a = 0;
      if (isnan(mz1p)) mz1p = 0;
      if (isnan(mz1a)) mz1a = 0;
      if (isnan(mz2a)) mz2a = 0;
      if (isnan(mz4a)) mz4a = 0;
      latitude = safe_atof(r[i++]);
      longitude = safe_atof(r[i++]);
      levelvalue = safe_atof(r[i++]);
      levelid = safe_atoi(r[i++]);
      alignid = safe_atoi(r[i++]);
      strcpy2(file, r[i++]);
      dt = safe_atof(r[i++]);
      numreset = safe_atoi(r[i++]);
      qcn_sensorid = safe_atoi(r[i++]);
      strcpy2(sw_version, r[i++]);
      strcpy2(os_type, r[i++]);
      qcn_quakeid = safe_atoi(r[i++]);
      time_filereq = safe_atof(r[i++]);
      received_file = safe_atoi(r[i++]);
      runtime_clock = safe_atof(r[i++]);
      runtime_cpu = safe_atof(r[i++]);
      varietyid = safe_atoi(r[i++]);
      flag = safe_atoi(r[i++]);
      hostipaddrid = safe_atoi(r[i++]);
      geoipaddrid = safe_atoi(r[i++]);
    }
};

#ifdef QCN_TRIGGER_MEMORY_TABLE
class DB_QCN_TRIGGER_MEMORY : public DB_BASE, public QCN_TRIGGER_MEMORY
{
public:
    DB_QCN_TRIGGER_MEMORY(DB_CONN* dc=0) :
        DB_BASE("qcn_trigger_memory", dc ? dc : &trigmem_db)  { clear(); }

    DB_ID_TYPE get_id() {return triggerid;}

    void db_print(char* buf)
    {
      char strLevelValue[32], strLevelID[16];
      if (levelid==0) {
        strcpy(strLevelValue, "NULL");
        strcpy(strLevelID, "NULL");
      }
      else {
        sprintf(strLevelValue, "%f", levelvalue);
        sprintf(strLevelID, "%d", levelid);
      }
      sprintf(buf,
        "db_name='%s',"
        "triggerid=%d,"
        "hostid=%d,"
        "ipaddr='%s',"
        "result_name='%s',"
        "time_trigger=%f,"
        "time_received=unix_timestamp(),"
        "time_sync=%f,"
        "sync_offset=%f,"
        "significance=%f,"
        "magnitude=%f,"
        "mxy1p=%f,"
        "mz1p=%f,"
        "mxy1a=%f,"
        "mz1a=%f,"
        "mxy2a=%f,"
        "mz2a=%f,"
        "mxy4a=%f,"
        "mz4a=%f,"
        "latitude=%16.9f,"
        "longitude=%16.9f,"
        "levelvalue=%s,"
        "levelid=%s,"
        "alignid=%d,"
        "file='%s',"
        "dt=%f,"
        "numreset=%d,"
        "qcn_sensorid=%d,"
        "varietyid=%d,"
        "qcn_quakeid=%d,"
        "hostipaddrid=%d,"
        "geoipaddrid=%d,"
        "posted=%d"
        ,
        db_name, triggerid, hostid, ipaddr, result_name, time_trigger, time_sync, sync_offset,
        significance, magnitude, 
        mxy1p,
        mz1p,
        mxy1a,
        mz1a,
        mxy2a,
        mz2a,
        mxy4a,
        mz4a,
        latitude, longitude, strLevelValue, strLevelID, alignid,
        file, dt, numreset, qcn_sensorid, varietyid, qcn_quakeid, hostipaddrid, geoipaddrid, posted ? 1 : 0
      );
    }

    void db_parse(MYSQL_ROW& r)
    {
      int i=0;
      clear();
      strcpy2(db_name, r[i++]);
      triggerid = safe_atoi(r[i++]);
      hostid = safe_atoi(r[i++]);
      strcpy2(ipaddr, r[i++]);
      strcpy2(result_name, r[i++]);
      time_trigger = safe_atof(r[i++]);
      time_received = safe_atof(r[i++]);
      time_sync = safe_atof(r[i++]);
      sync_offset = safe_atof(r[i++]);
      significance = safe_atof(r[i++]);
      if (isnan(significance)) significance = 0;
      magnitude = safe_atof(r[i++]);
      if (isnan(magnitude)) magnitude = 0;  // some reason sig &  or mag can be NaN
      mxy1p = safe_atof(r[i++]);
      mz1p = safe_atof(r[i++]);
      mxy1a = safe_atof(r[i++]);
      mz1a = safe_atof(r[i++]);
      mxy2a = safe_atof(r[i++]);
      mz2a = safe_atof(r[i++]);
      mxy4a = safe_atof(r[i++]);
      mz4a = safe_atof(r[i++]);
      if (isnan(mxy1p)) mxy1p = 0;
      if (isnan(mxy1a)) mxy1a = 0;
      if (isnan(mxy2a)) mxy2a = 0;
      if (isnan(mxy4a)) mxy4a = 0;
      if (isnan(mz1p)) mz1p = 0;
      if (isnan(mz1a)) mz1a = 0;
      if (isnan(mz2a)) mz2a = 0;
      if (isnan(mz4a)) mz4a = 0;
      latitude = safe_atof(r[i++]);
      longitude = safe_atof(r[i++]);
      levelvalue = safe_atof(r[i++]);
      levelid = safe_atoi(r[i++]);
      alignid = safe_atoi(r[i++]);
      strcpy2(file, r[i++]);
      dt = safe_atof(r[i++]);
      numreset = safe_atoi(r[i++]);
      qcn_sensorid = safe_atoi(r[i++]);
      varietyid = safe_atoi(r[i++]);
      qcn_quakeid = safe_atoi(r[i++]);
      hostipaddrid = safe_atoi(r[i++]);
      geoipaddrid = safe_atoi(r[i++]);
      posted = (bool) safe_atoi(r[i++]);
    }
};
#endif

class DB_QCN_QUAKE: public DB_BASE, public QCN_QUAKE
{
public:
    DB_QCN_QUAKE(DB_CONN* dc=0) :
          DB_BASE("qcn_quake", dc ? dc : &boinc_db)  { clear(); }

    DB_ID_TYPE get_id() {return id;}

    void db_print(char* buf)
    {
      //ESCAPE(description);
      //ESCAPE(url);
      //ESCAPE(guid);
      sprintf(buf,
        "id=%d,"
        "time_utc=%f,"
        "magnitude=%f,"
        "depth_km=%f,"
        "latitude=%f,"
        "longitude=%f,"
        "description='%s',"
        "processed=%d,"
        "url='%s',"
        "guid='%s'"
        ,
        id, time_utc, magnitude, depth_km, latitude, longitude, description, processed, url, guid 
      );
      //UNESCAPE(description);
      //UNESCAPE(url);
      //UNESCAPE(guid);
    }

    void db_parse(MYSQL_ROW& r)
    {
      int i=0;
      clear();
      id = safe_atoi(r[i++]);
      time_utc = safe_atof(r[i++]);
      magnitude = safe_atof(r[i++]);
      depth_km = safe_atof(r[i++]);
      latitude = safe_atof(r[i++]);
      longitude = safe_atof(r[i++]);
      strcpy2(description, r[i++]);
      processed = safe_atoi(r[i++]);
      strcpy2(url, r[i++]);
      strcpy2(guid, r[i++]);
    }
};

#endif  // ifndef _QCN_TRIGGER_H
