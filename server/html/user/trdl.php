<?php
require_once("../inc/util.inc");
require_once("../inc/db.inc");
require_once("../inc/sqlquerystring.inc");
require_once("../project/common.inc");

db_init();

set_time_limit(600);

// make sure they're logged in
$user = get_logged_in_user(true);

// if no querystring then we are coming in "fresh" so just show a small subset ie usgs-detected quakes
$bNoQuery = ($_SERVER["QUERY_STRING"] == null);

// Check if the user is on the administrative list:
$auth = qcn_admin_user_check($user);
$auth = true;
// authenticate admin-level user
// qcn_admin_user_auth($user, true);

// archive cutoff time is two months prior to the first of the current month
//$queryArchiveTime = "SELECT unix_timestamp( concat(year(now()), '/', month(now()), '/01 00:00:00') ) - (60 * 24 * 3600) archive_time";
$try_replica = false;
$unixtimeArchive = mktime(0, 0, 0, date("n"), 1, date("Y")) - (60*24*3600); 
$queryCount = 0;

    $config = get_config();
        $user = parse_config($config, "<db_user>");
        $pass = parse_config($config, "<db_passwd>");
    $host = null;
    if ($try_replica == true) {
          $host = parse_config($config, "<replica_db_host>");
    }
    if ($host == null) {
          $host = parse_config($config, "<db_host>");
    }
    if ($host == null) {
        $host = "localhost";
    }
    $link = mysql_pconnect($host, $user, $pass);
    if (!$link) {
        return 1;
    }
$db_name = parse_config($config, "<db_name>");
$db_archive = "";

if ($db_name == "sensor") {
  $db_archive = "sensor_archive";
}
else if ($db_name== "continual") {
  $db_archive = "continual_archive";
}

$queryBase = "select q.id as quakeid, q.time_utc as quake_time, q.magnitude as quake_magnitude, 
q.depth_km as quake_depth, q.latitude as quake_lat, 
q.longitude as quake_lon, q.description, q.url, q.guid,
round(lat_lon_distance_m(q.latitude, q.longitude, t.latitude, t.longitude) / 1000.0, 3) as quake_distance_km,
t.id as triggerid, t.hostid, t.ipaddr, h.domain_name hostname, t.result_name, t.time_trigger as trigger_time, 
(t.time_received-t.time_trigger) as delay_time, t.time_sync as trigger_sync,
t.sync_offset, t.significance, t.magnitude as trigger_mag, 
t.latitude as trigger_lat, t.longitude as trigger_lon, t.file as trigger_file, t.dt as delta_t,
t.numreset, s.description as sensor_description, t.sw_version, t.qcn_quakeid, t.time_filereq as trigger_timereq, 
t.received_file, REPLACE_ARCHIVE is_archive, t.varietyid, q.url quake_url, if(t.geoipaddrid=0, 'N', 'Y') is_geoip 
FROM REPLACE_DB.qcn_trigger t LEFT OUTER JOIN sensor.qcn_quake q ON t.qcn_quakeid = q.id
   LEFT JOIN sensor.qcn_sensor s ON t.qcn_sensorid = s.id 
   LEFT JOIN REPLACE_HOSTDB.host h ON t.hostid=h.id
";

// full querystring
// /continual_dl/trdl.php?cbCSV=1&cbUseLat=1&LatMin=-39&LatMax=-30&LonMin=-76&LonMax=-69&cbUseSensor=1&qcn_sensorid=100&cbUseTime=1&date_start=2010-03-24&time_hour_start=0&time_minute_start=0&date_end=2010-03-25&time_hour_end=0&time_minute_end=0&rb_sort=ttd

// sort order options: tta/d  hosta/d  maga/d lata/d lona/d
// get the archive time
$queryTime = "SELECT value_int+1 as archive_time FROM sensor.qcn_constant WHERE description='ArchiveTime'";
$result = mysql_query($queryTime);
if ($result) {
  $row = mysql_fetch_row($result);
  $unixtimeArchive = $row[0];
  mysql_free_result($result);
}

// first off get the sensor types
$sqlsensor = "select id,description from qcn_sensor order by id";
$result = mysql_query($sqlsensor);
$i = 0;
$arrSensor = array();
if ($result) {
    while ($res = mysql_fetch_array($result)) {
       $arrSensor[$i] = $res;
       $i++;
    }
    mysql_free_result($result);
}

$detail = null;
$show_aggregate = false;

$q = new SqlQueryString();

// start $_GET

$nresults = get_int("nresults", true);
$last_pos = get_int("last_pos", true);

$bUseCSV = get_int("cbUseCSV", true);
$bUseArchive = get_int("cbUseArchive", true);
$bUseFile  = get_int("cbUseFile", true);
$bUseGeoIP = get_int("cbUseGeoIP", true);
$bUseQuake = get_int("cbUseQuake", true);
$bUseQCNQuake = get_int("cbUseQCNQuake", true);
$bUseLat   = get_int("cbUseLat", true);
$bUseSensor = get_int("cbUseSensor", true);
$bUseTime  = get_int("cbUseTime", true);
$bUseHost = get_int("cbUseHost", true);
$bDownloadAll = get_int("cbDownloadAll", true);
$strHostID = get_int("HostID", true);
$strHostName = get_str("HostName", true);

$quake_mag_min = get_str("quake_mag_min", true);
$qcn_quake_mag_min = get_str("qcn_quake_mag_min", true);

$qcn_sensorid = get_int("qcn_sensorid", true);
$dateStart = get_str("date_start", true);
$dateEnd   = get_str("date_end", true);

$strLonMin = get_str("LonMin", true);
$strLonMax = get_str("LonMax", true);
$strLatMin = get_str("LatMin", true);
$strLatMax = get_str("LatMax", true);

$timeHourStart   = get_int("time_hour_start", true);
$timeMinuteStart = get_int("time_minute_start", true);

$timeHourEnd   = get_int("time_hour_end", true);
$timeMinuteEnd = get_int("time_minute_end", true);

$sortOrder = get_str("rb_sort", true);

$bVarietyNormal = get_str("cbVarietyNormal", true);
$bVarietyPing = get_str("cbVarietyPing", true);
$bVarietyContinual = get_str("cbVarietyContinual", true);

if ($bVarietyNormal == "" && $bVarietyPing == "" && $bVarietyContinual == "") {
   $bVarietyNormal = "0"; // at least show normal trigs
   if ($db_name == "continual") {
     $bVarietyContinual = "2"; // also show continual trigs
   }
}
$search_show = get_str("search_show", true);
$plot_map = get_str("plot_map", true);
if ($plot_map == "y") {
 $search_show="n";
}

if ($search_show == "n" || $search_show == "N") {
 $view_search = false;
 if (!$nresults) {$nresults=50;}
} else {
 $view_search = true;
}

/*if ($strLatMin =="") {$strLatMin =  "-90.0";}
if ($strLatMax =="") {$strLatMax =  " 90.0";}
if ($strLonMin =="") {$strLonMin = "-180.0";}
if ($strLonMax =="") {$strLonMax =  "180.0";}*/


// end $_GET

//if (!$quake_mag_min && (!$timeHourStart || !$dateStart)) $quake_mag_min = "3.0";  // set minimum quake mag cutoff
if (!$quake_mag_min) $quake_mag_min = "3.0";  // set minimum quake mag cutoff
if (!$qcn_quake_mag_min) $qcn_quake_mag_min = "3.0";  // set minimum quake mag cutoff


// make sure these are in the right order, as the sql "between" will fail if max < min!
// people may forget that lon -76 is less than -72 as it may make more sense to think -72 to -76
if ($strLonMax < $strLonMin) {
   $temp = $strLonMax;
   $strLonMax = $strLonMin;
   $strLonMin = $temp;
}

if ($strLatMax < $strLatMin) {
   $temp = $strLatMax;
   $strLatMax = $strLatMin;
   $strLatMin = $temp;
}

if (!$sortOrder) $sortOrder = "ttd";  // triger time desc is default sort order

if ($bDownloadAll)
{
$nresults = 1e9;
}
if (!$nresults) $nresults = 200;
if ($nresults) {
    $entries_to_show = $nresults;
} else {
    $entries_to_show = 100;
}
$page_entries_to_show = $entries_to_show;

if ($last_pos) {
    $start_at = $last_pos;
} else {
    $start_at = 0;
}

//page_head("QCN Trigger Listing");
echo "<html><head>
<script type=\"text/javascript\" src=\"calendarDateInput.js\">

/***********************************************
* Jason's Date Input Calendar- By Jason Moon http://calendar.moonscript.com/dateinput.cfm
* Script featured on and available at http://www.dynamicdrive.com
* Keep this notice intact for use.
***********************************************/

</script>
  <title>QCN Trigger Listing</title>
</head><body " . BODY_COLOR . ">\n";
// echo "<font size=\"1\">";
//  echo "<table><tr " . TITLE_COLOR . "><td>" . TITLE_FONT . "<font size=\"6\"><b><a href=\"trdl.php\">".PROJECT.":</a>  QCN Trigger Listing </b></font></td></tr></table>\n";


// if no constraints then at least use quakes as otherwise we'll have too many i.e. a million triggers
//if (!$bUseFile && !$bUseQuake && !$bUseQCNQuake && !$bUseLat && !$bUseTime && !$bUseSensor) $bUseQuake = 1;
// actually use time constraint, if quake page use the
//if (!$bUseFile && !$bUseQuake && !$bUseQCNQuake && !$bUseLat && !$bUseTime && !$bUseSensor) $bUseTime = 1;
if ($bNoQuery) {
  $bUseFile = 1;
  $bUseGeoIP = 1;
  $bUseQuake = 1;
  $bUseQCNQuake = 1;
  $quake_mag_min = 3.0;
  $qcn_quake_mag_min = 3.0;
}

if (!$bUseFile && $bDownloadAll) $bUseFile = 1; // if we want to download, set the file recv flag to true

// set last four hours for start, current time + 1 for end
$timeStart = gmdate("U", time() - (3600*4));
$timeEnd = gmdate("U", time() + 3600);
if (!$dateStart) {
  $dateStart = date("Y-m-d", $timeStart);
  // now set the times based on timeStart & timeEnd
  $timeHourStart = date("H", $timeStart);
  $timeMinuteStart = "00"; //date("i", $timeStart);
}
if (!$dateEnd) {
  $dateEnd = date("Y-m-d", $timeEnd);
  $timeHourEnd   = date("H", $timeEnd);
  $timeMinuteEnd = "00";
}

if (! $view_search) {
 echo "<input type=\"hidden\" id=\"cbUseFile\" name=\"cbUseFile\" value=\"$bUseFile\">";
 echo "<input type=\"hidden\" id=\"cbUseQuake\" name=\"cbUseQuake\" value=\"$bUseQuake\">";
 echo "<input type=\"hidden\" id=\"cbUseQCNQuake\" name=\"cbUseQCNQuake\" value=\"$bUseQCNQuake\">";
 echo "<input type=\"hidden\" id=\"quake_mag_min\" name=\"quake_mag_min\" value=\"$quake_mag_min\" size=\"4\">";
 echo "<input type=\"hidden\" id=\"qcn_quake_mag_min\" name=\"qcn_quake_mag_min\" value =\"$qcn_quake_mag_min\" size=\"4\">";
 echo "<input type=\"hidden\" id=\"cbUseTime\" name=\"cbUseTime\" value=\"1\" >";
 echo "<input type=\"hidden\" name=\"time_hour_start\" value=\"".$timeHourStart."\">";
 echo "<input type=\"hidden\" name=\"time_hour_start\" value=\"".$timeMinStart."\">";
 echo "<input type=\"hidden\" name=\"time_hour_start\" value=\"".$timeHourEnd."\">";
 echo "<input type=\"hidden\" name=\"time_hour_start\" value=\"".$timeMinEnd."\">";
 echo "<input type=\"hidden\" name=\"date_start\" value=\"".$dateStart."\">";
 echo "<input type=\"hidden\" name=\"date_end\" value=\"".$dateEnd."\">";

} else {
echo "<form name='formSelect' method=\"get\" action=trdl.php >";
//echo "<HR>Constraints:<br><br>";


echo "<table align=\"center\" border=\"1\" bgcolor=\"#ddd\"><tr><td>";
echo "<h1><center>Search QCN Data:</center></h1>";
echo "
<table bgcolor=\"#fff\"><tr><td width=\"300\">
  <p><input type=\"checkbox\" id=\"cbUseLat\" name=\"cbUseLat\" value=\"1\" " . ($bUseLat ? "checked" : "") . ">Location Search: 
  <BR>
  <table>
    <tr>
      <td></td>
      <td bgcolor=\"#DDD\">North Lat:<br> <input id=\"LatMax\" name=\"LatMax\" value=\"" . $strLatMax . "\" size=\"8\"></td>
      <td></td>
    </tr>
    <tr>
      <td bgcolor=\"#DDD\">West Lon:<br> <input id=\"LonMin\" name=\"LonMin\" value=\"" . $strLonMin . "\" size=\"8\"></td>
      <td align=\"center\">+/- 90 Lat <br>+/-180 Lon</td>
      <td bgcolor=\"#DDD\">East Lon:<br> <input id=\"LonMax\" name=\"LonMax\" value=\"" . $strLonMax . "\" size=\"8\"></td>
    </tr>
    <tr>
      <td></td>
      <td bgcolor=\"#DDD\">South Lat:<br> <input id=\"LatMin\" name=\"LatMin\" value=\"" . $strLatMin . "\" size=\"8\"></td>
      <td></td>
    </tr>
  </table><br>";


echo "  <input type=\"checkbox\" id=\"cbUseFile\" name=\"cbUseFile\" value=\"1\" " . ($bUseFile ? "checked" : "") . "> Only Show If Files Received
   <p><input type=\"checkbox\" id=\"cbUseGeoIP\" name=\"cbUseGeoIP\" value=\"1\" " . ($bUseGeoIP ? "checked" : "") . "> Include GeoIP Only Triggers 
  <p><input type=\"checkbox\" id=\"cbUseQuake\" name=\"cbUseQuake\" value=\"1\" " . ($bUseQuake ? "checked" : "") . "> Match USGS Quakes:&nbsp;
  Mag >= &nbsp;<input id=\"quake_mag_min\" name=\"quake_mag_min\" value=\"$quake_mag_min\" size=\"4\">

  <p><input type=\"checkbox\" id=\"cbUseQCNQuake\" name=\"cbUseQCNQuake\" value=\"$bUseQCNQuake\" " . ($bUseQCNQuake ? "checked" : "") . "> Match QCN Quakes:&nbsp;Mag >=&nbsp;<input id=\"qcn_quake_mag_min\" name=\"qcn_quake_mag_min\" value =\"$qcn_quake_mag_min\" size=\"4\">";

/* Max number of Triggers per Page:*/
echo "<p><input type=\"checkbox\" disabled=\"disabled\" checked=\"checked\"> Max Data Per Page: &nbsp;&nbsp;&nbsp;&nbsp;&nbsp;<input id=\"nresults\" name=\"nresults\" value=\"$nresults\" size=\"4\">\n";

/* Sort data in this order: */
echo "<p><input type=\"checkbox\" disabled=\"disabled\" checked=\"checked\">Sort Order:";
echo "<ul><h7><select name=\"rb_sort\" id=\"rb_sort\">
";
   echo "<option value=\"maga\" ";
   if ($sortOrder == "maga") echo "selected";
   echo ">Magnitude (Ascending)";

   echo "<option value=\"magd\" ";
   if ($sortOrder == "magd") echo "selected";
   echo ">Magnitude (Descending)";

   echo "<option value=\"tta\" ";
   if ($sortOrder == "tta") echo "selected";
   echo ">Trigger Time (Earliest First)";

   echo "<option value=\"ttd\" ";
   if ($sortOrder == "ttd") echo "selected";
   echo ">Trigger Time (Latest First)";

   echo "<option value=\"lata\" ";
   if ($sortOrder == "lata") echo "selected";
   echo ">Latitude (Ascending)";

   echo "<option value=\"latd\" ";
   if ($sortOrder == "latd") echo "selected";
   echo ">Latitude (Descending)";

   echo "<option value=\"lona\" ";
   if ($sortOrder == "lona") echo "selected";
   echo ">Longitude (Ascending)";

   echo "<option value=\"lond\" ";
   if ($sortOrder == "lond") echo "selected";
   echo ">Longitude (Descending)";

   echo "<option value=\"hosta\" ";
   if ($sortOrder == "hosta") echo "selected";
   echo ">Host ID (Ascending)";

   echo "<option value=\"hostd\" ";
   if ($sortOrder == "hostd") echo "selected";
   echo ">Host ID (Descending)";

   echo "<option value=\"qda\" ";
   if ($sortOrder == "qda") echo "selected";
   echo ">Quake Distance (Ascending)";

   echo "<option value=\"qdd\" ";
   if ($sortOrder == "qdd") echo "selected";
   echo ">Quake Distance (Decending)";

   echo "</select></ul>\n";


echo "</td>\n";


//Show specific host
echo "
<td>
  <input type=\"checkbox\" id=\"cbUseHost\" name=\"cbUseHost\" value=\"1\" " . ($bUseHost? "checked" : "") . "> Show Specific Host (enter host ID # or host name)
<ul>
    <table><tr><td>Host ID:  </td><td><input id=\"HostID\"   name=\"HostID\"   value=\"$strHostID\"  ></td></tr>
           <tr><td>Host Name:<BR>(% = wildcard)</td><td><input id=\"HostName\" name=\"HostName\" value=\"$strHostName\"></td></tr></table>
</ul>";



/* Show time constraint */
echo "
  <input type=\"checkbox\" id=\"cbUseTime\" name=\"cbUseTime\" value=\"1\" " . ($bUseTime ? "checked" : "") . "> Use Time Constraint
<BR>
";
/*
echo "<BR><font color=red><B>Please note that triggers older than two months are temporarily not available.</b></font><BR><BR>";
*/
echo "<ul><table><tr><td>Start Time:</td><td>";



echo "<script>DateInput('date_start', true, 'YYYY-MM-DD', '$dateStart')</script></td><td>";


echo "<select name=\"time_hour_start\" id=\"time_hour_start\">";
for ($i = 0; $i < 24; $i++) {
   echo "<option value=$i ";
   if ($i == $timeHourStart) echo "selected";
   echo ">" . sprintf("%02d", $i);
}
echo "</select>";

echo ":<select name=\"time_minute_start\" id=\"time_minute_start\">";
for ($i = 0; $i < 60; $i++) {
   echo "<option value=$i ";
   if ($i == $timeMinuteStart) echo "selected";
   echo ">" . sprintf("%02d", $i);
}
echo "</select> (UTC)";

echo "</td><tr>
<tr><td>End Time</td><td>";

echo "<script>DateInput('date_end', true, 'YYYY-MM-DD', '$dateEnd')</script></td><td>";


echo "<select name=\"time_hour_end\">";
for ($i = 0; $i < 24; $i++) {
   echo "<option value=$i ";
   if ($i == $timeHourEnd) echo "selected";
   echo ">" . sprintf("%02d", $i);
}
echo "</select>";

echo ":<select name=\"time_minute_end\" id=\"time_minute_end\">";
for ($i = 0; $i < 60; $i++) {
   echo "<option value=$i ";
   if ($i == $timeMinuteEnd) echo "selected";
   echo ">" . sprintf("%02d", $i);
}
echo "</select> (UTC)\n </td></tr></table></UL> \n ";


/* Select Sensor Type: */

echo "<p><input type=\"checkbox\" id=\"cbUseSensor\" name=\"cbUseSensor\" value=\"1\" " . ($bUseSensor ? "checked" : "") . "> Use Sensor Type:\n";
echo "<ul>\n";
echo "<select name=\"qcn_sensorid\" id=\"qcn_sensorid\">";
echo "<font size=\"1\">";
  for ($i = 0; $i < sizeof($arrSensor); $i++)  {
     echo "<option value=" . $arrSensor[$i][0];
     if ($qcn_sensorid == $arrSensor[$i][0]) echo " selected";
     echo ">" . $arrSensor[$i][1] . "\n";
  }
echo "</select>\n </ul>\n";

/* Data Type:*/
echo "<p><input type=\"checkbox\" id=\"cbVarietyNormal\" name=\"cbVarietyNormal\" value=\"0\" "
   . ($bVarietyNormal == "0" ? "checked" : "") . "> Show Triggered Data \n";

echo "<p><input type=\"checkbox\" id=\"cbVarietyPing\" name=\"cbVarietyPing\" value=\"1\" "
   . ($bVarietyPing == "1" ? "checked" : "") . "> Show State of Health Data \n";
if ($db_name == "continual") { // only show on continual page
 echo "<p><input type=\"checkbox\" id=\"cbVarietyContinual\" name=\"cbVarietyContinual\" value=\"2\" "
    . ($bVarietyContinual == "2" ? "checked" : "") . "> Show Continual Data \n";
}

echo "</ul>\n";

echo "<p><input type=\"checkbox\" id=\"cbUseCSV\" name=\"cbUseCSV\" value=\"1\" " . ($bUseCSV? "checked" : "") . "> Create Text/CSV File of Triggers?";
echo "<p><input type=\"checkbox\" id=\"cbDownloadAll\" name=\"cbDownloadAll\" value=\"1\"> Setup Page to Download All?";
echo "</td></tr></table>";  // End of inner table



// end the form
echo "<center><input type=\"submit\" value=\"Submit Constraints\" name=\"btnConstraints\" id=\"btnConstraints\" />
</center>\n";
echo "</form>";
if ($db_name == "continual") {
echo "<center><a href=\"" . BASEURL . "/continual/trdl.php\">Start Over</a></center>\n";
} else {
echo "<center><a href=\"" . BASEURL . "/sensor/trdl.php\">Start Over</a></center>\n";
}


echo "</td></tr></table>";  // End of outer table
}


echo " <H7>";

$varStr = "";
if ($bVarietyNormal != "") $varStr = $bVarietyNormal;
if ($bVarietyPing != "") if ($varStr == "") $varStr = $bVarietyPing; else $varStr .= ",$bVarietyPing";
if ($bVarietyContinual != "") if ($varStr == "") $varStr = $bVarietyContinual; else $varStr .= ",$bVarietyContinual";
if ($varStr == "") $varStr = "0";
// alwayus at least have variety = 1
$whereString = "t.varietyid in ($varStr)";


if ($bUseFile) {
   $whereString .= " AND t.received_file = 100 ";
}

if (!$bUseGeoIP) {
   $whereString .= " AND t.geoipaddrid = 0 ";
}

if ($bUseHost) {
  if ($strHostID) {
     $whereString .= " AND t.hostid = " . $strHostID;
  }
  else if ($strHostName) {
    if (strpos($strHostName, "%")===FALSE) { // wildcard
     $whereString .= " AND h.domain_name = '" . $strHostName . "'";
    }
    else {
     $whereString .= " AND h.domain_name like '" . $strHostName . "'";
    }
  }
}

if ($bUseQuake || $bUseQCNQuake) {
  if ($bUseQuake && !$bUseQCNQuake) {
     $whereString .= " AND t.qcn_quakeid>0 AND q.magnitude >= " . $quake_mag_min;
  }
  else if (!$bUseQCNQuake && $bUseQCNQuake) {
   $whereString .= " AND t.qcn_quakeid>0 AND q.guid like 'QCN_%' AND q.magnitude >= ". $qcn_quake_mag_min;
  }
  else { // both
    $min = 1.;
    if ($quake_mag_min > $qcn_quake_mag_min) $min = $quake_mag_min;
    else $min = $qcn_quake_mag_min;
    $whereString .= " AND t.qcn_quakeid > 0 and q.magnitude >= $min";
  }
}

if ($bUseLat) {
   $whereString .= " AND t.latitude BETWEEN $strLatMin AND $strLatMax AND t.longitude BETWEEN $strLonMin AND $strLonMax ";
}

if ($bUseSensor) {
   $whereString .= " AND t.qcn_sensorid=$qcn_sensorid ";
}

$unixtimeStart = 0;
$unixtimeEnd = 0;

if (!$dateStart) {
  $dateStart = date("Y/m/d", $timeStart);
  // now set the times based on timeStart & timeEnd
  $timeHourStart = date("H", $timeStart);
  $timeMinuteStart = "00"; //date("i", $timeStart);
}
if (!$dateEnd) {
  $dateEnd = date("Y/m/d", $timeEnd);
  $timeHourEnd   = date("H", $timeEnd);
  $timeMinuteEnd = "00";
}

//YYYY-MM-DD
$unixtimeStart = mktime($timeHourStart, $timeMinuteStart, 0, substr($dateStart, 5, 2), substr($dateStart, 8, 2), substr($dateStart, 0, 4));
$unixtimeEnd   = mktime($timeHourEnd,   $timeMinuteEnd,   0, substr($dateEnd  , 5, 2), substr($dateEnd  , 8, 2), substr($dateEnd,   0, 4));
    
$bUseArchive = "";
if ($bUseTime && $unixtimeStart && $unixtimeEnd) {
   $bUseArchive = ($unixtimeStart < $unixtimeArchive || $unixtimeEnd < $unixtimeArchive);
   $whereString .= " AND t.time_trigger BETWEEN $unixtimeStart AND $unixtimeEnd ";
    //echo "<BR>HAHA<BR>" . $dateStart . "<BR>" . $unixtimeStart . "<BR><BR>" . $dateEnd . "<BR>" . $unixtimeEnd . "<BR><BR>";
}

//print $unixtimeStart . "<BR>" . $unixtimeEnd . "<BR>" . $unixtimeArchive;

echo "<input type=\"hidden\" id=\"cbUseArchive\" name=\"cbUseArchive\" value=\"" . ($bUseArchive ? "1" : "") . "\"> ";
echo "<input type=\"hidden\" id=\"db_name\" name=\"db_name\" value=\"" . $db_name . "\"> ";

$sortString = "trigger_time DESC";
switch($sortOrder)
{
   case "maga":
      $sortString = "quake_magnitude ASC, trigger_time DESC";
      break;
   case "magd":
      $sortString = "quake_magnitude DESC, trigger_time DESC";
      break;
   case "tta":
      $sortString = "trigger_time ASC";
      break;
   case "ttd":
      $sortString = "trigger_time DESC";
      break;
   case "lata":
      $sortString = "trigger_lat ASC, trigger_lon ASC";
      break;
   case "latd":
      $sortString = "trigger_lat DESC, trigger_lon DESC";
      break;
   case "lona":
      $sortString = "trigger_lon ASC, trigger_lat ASC";
      break;
   case "lond":
      $sortString = "trigger_lon DESC, trigger_lat DESC";
      break;
   case "hosta":
      $sortString = "hostid ASC";
      break;
   case "hostd":
      $sortString = "hostid DESC";
      break;
   case "qda":
      $sortString = "quake_distance_km ASC, trigger_time DESC";
      break;
   case "qdd":
      $sortString = "quake_distance_km DESC, trigger_time DESC";
      break;
}

// really need to look at archive table too
$query = str_replace("REPLACE_HOSTDB", $db_name, $queryBase);
$query = str_replace("REPLACE_DB", $db_name, $query);
$query = str_replace("REPLACE_ARCHIVE", "0", $query);
if ($bUseArchive) {
   $queryArc = str_replace("REPLACE_DB", $db_archive, $queryBase);
   $queryArc = str_replace("REPLACE_HOSTDB", $db_name, $queryArc);
   $query .= " WHERE " . $whereString . " UNION " . $queryArc . " WHERE " . $whereString . " ORDER BY " . $sortString;
   $query = str_replace("REPLACE_ARCHIVE", "1", $query);
}
else {
  $query .= 
     " WHERE " . $whereString
       . " ORDER BY " . $sortString
     ;
}

//$main_query = $q->get_select_query($entries_to_show, $start_at);
        if (!$bUseCSV && !$bDownloadAll && $entries_to_show) {
            if ($start_at) {
                $main_query = $query . " limit $start_at,$entries_to_show";
            } else {
                $main_query = $query . " limit $entries_to_show";
            }
        } else {
            $main_query = $query;
        }

//$count = 1e6;
//print "<BR><BR>$query<BR><BR>";

if (!$bUseCSV && !$bDownloadAll) { // don't count for big query
$count = query_count($query);

// CMC HERE
//print "<BR><BR>" . $query . "<BR><BR>";

if ($count < $start_at + $entries_to_show) {
    $entries_to_show = $count - $start_at;
}

$last = $start_at + $entries_to_show;
} // query count

// For display, convert query string characters < and > into 'html form' so
// that they will be displayed.
//
//$html_text=str_replace('<', '&lt;', str_replace('>', '&gt;', $main_query));
//echo "<p>Query: <b>$html_text</b><p>\n";

echo "
<script type=\"text/javascript\">
   
function SetAllCheckBoxes(FormName, FieldName, CheckValue)
{
	if(!document.forms[FormName])
		return;
	var objCheckBoxes = document.forms[FormName].elements[FieldName];
	if(!objCheckBoxes)
		return;
	var countCheckBoxes = objCheckBoxes.length;
	if(!countCheckBoxes)
		objCheckBoxes.checked = CheckValue;
	else
		// set the check value for all check boxes
		for(var i = 0; i < countCheckBoxes; i++)
	           if (! objCheckBoxes[i].disabled)
                      objCheckBoxes[i].checked = CheckValue;
}
</script>
<HR>
";

 
$start_1_offset = $start_at + 1;
if (!$bUseCSV && !$bDownloadAll) {
echo "
    <p>$count records match the query.
    Displaying $start_1_offset to $last.<p>
";
}

$url = $q->get_url("trdl.php");
if ($detail) {
    $url .= "&detail=$detail";
}
$queryString = "&nresults=$page_entries_to_show"
       . "&cbUseHost=$bUseHost"
       . "&cbUseArchive=$bUseArchive"
       . "&cbUseFile=$bUseFile"
       . "&cbUseCSV=$bUseCSV"
       . "&cbUseQuake=$bUseQuake"
       . "&cbUseQCNQuake=$bUseQCNQuake"
       . "&cbUseLat=$bUseLat"
       . "&cbUseTime=$bUseTime"
       . "&cbUseSensor=$bUseSensor"
       . "&qcn_sensorid=$qcn_sensorid"
       . "&date_start=$dateStart"
       . "&date_end=$dateEnd"
       . "&LonMin=$strLonMin"
       . "&LonMax=$strLonMax"
       . "&LatMin=$strLatMin"
       . "&LatMax=$strLatMax"
       . "&quake_mag_min=$quake_mag_min"
       . "&qcn_quake_mag_min=$qcn_quake_mag_min"
       . "&time_hour_start=$timeHourStart"
       . "&time_minute_start=$timeMinuteStart"
       . "&time_hour_end=$timeHourEnd"
       . "&time_minute_end=$timeMinuteEnd"
       . "&HostID=$strHostID"
       . "&HostName=$strHostName"
       . "&rb_sort=$sortOrder"
       . "&plot_map=$plot_map"
       . "&search_show=$search_show";

//echo "<hr>$url<hr><br>\n";
if ($start_at || $last < $count) {
    echo "<table border=\"1\"><tr><td width=\"100\">";
    if ($start_at) {
        $prev_pos = $start_at - $page_entries_to_show;
        if ($prev_pos < 0) {
            $prev_pos = 0;
        }
        echo "
            <a href=\"$url&last_pos=$prev_pos" . $queryString . "\">Previous $page_entries_to_show</a><br>
        ";
    }
    echo "</td><td width=100>";
    if ($last < $count) {
        echo "
            <a href=\"$url&last_pos=$last" . $queryString . "\">Next $page_entries_to_show</a><br>
        ";
    }
    echo "</td></tr></table>";
}

echo "<p>\n";


if ($bUseCSV) {   
   // tmp file name tied to user ID & server time
   $fileTemp = sprintf("data/%ld_u%d.csv", time(), $user->id);
   $ftmp = fopen($fileTemp, "w");
   if ($ftmp) {
      fwrite($ftmp, qcn_trigger_header_csv($auth));
   }
   else {
      $fileTemp = ""; // to check for status later on down
   }
}


$bResultShow = false;
// do an unbuffered query for huge selections
if ($bDownloadAll)
  $result = mysql_unbuffered_query($main_query);
else
  $result = mysql_query($main_query);

if ($result) {
$arrSensor = array();
   $bResultShow = true;
    if ($auth && $plot_map!="y") {
       echo "(<font=small>'Request Files' = send msg to host to upload files to QCN,   'Batch Download' = request download existing file) <BR><BR>\n";
    }
    echo "<form name=\"formDetail\" method=\"post\" action=trdlreq.php >";

    printDownloadOptions($bUseCSV, $ftmp, $fileTemp, $bResultShow, $bUseArchive, $db_name, $auth, $plot_map);
    start_table();
    if (!$bUseCSV && !$ftmp && !$plot_map && !$bDownloadAll) qcn_trigger_header($auth);
    $iii = 0;
    $ii = 0;
    $ie = 0;
    $mag_last=0;
    $queryCount = 0;
    while ($res = mysql_fetch_object($result)) {
        
        if ($bUseCSV && $ftmp) {
           fwrite($ftmp, qcn_trigger_detail_csv($res,$auth,$user));
        }
        else { 
           ++$ii;
           if ($iii == 0) {
            ++$iii;
            $bg_color="#ffffff";
           } else {
            $iii = 0;
            $bg_color="#dddddd";
           }
           if ($plot_map=="y") {
           $ie++;
           $arrSensor[$ie]=$res;
//           view_waveform_image_and_header($flocs,$res,$mag_last,$message1);
           } else {
	       qcn_trigger_detail($res,$bg_color,$auth,$user, $bDownloadAll);
               $queryCount++;
            }
        }
    }


if ($bDownloadAll) {
   echo "<p><font color='red'>$queryCount records queued for download processing.  Press 'Submit All Requests' to Confirm.</font>";
}

    end_table();
    mysql_free_result($result);
    if ($plot_map=="y") {
      $file_out = BASEPATH."/qcnwp/earthquakes/view/.temp.map.".rand(0,10000);
      $flocs = fopen($file_out,'w'); 
      view_waveform_images_and_headers($flocs,$arrSensor,$mag_last,$ie);
      fclose($flocs);
      echo "<iframe src=\"" . BASEURL . "/earthquakes/view/qcn_map_network.php?ifile=".$file_out."\" frameborder=\"0\" scrolling=\"no\" width=\"100%\" height=\"750\"></iframe>";
    }
} else {
    echo "<h2>No results found - try different query settings</h2>\n";
}

  //printDownloadOptions($bUseCSV, $ftmp, $fileTemp, $bResultShow, $bUseArchive, $db_name, $auth, $plot_map);

  echo "</form>\n";
  //&nbsp&nbsp&nbsp&nbsp
  //<input type=\"submit\" id=\"submitDownload\" name=\"submitDownload\" value=\"Download All Available Files for Query\" />\n";


  if ($start_at || $last < $count) {
    echo "<table border=\"1\"><tr><td width=\"100\">";
    if ($start_at) {
        $prev_pos = $start_at - $page_entries_to_show;
        if ($prev_pos < 0) {
            $prev_pos = 0;
        }
        echo "
            <a href=\"$url&last_pos=$prev_pos" . $queryString . "\">Previous $page_entries_to_show</a><br>
        ";
    }
    echo "</td><td width=100>";
    if ($last < $count) {
        echo "
            <a href=\"$url&last_pos=$last" . $queryString . "\">Next $page_entries_to_show</a><br>
        ";
    }
    echo "</td></tr></table>";
  }

    if ($bUseCSV && $ftmp) {
      fclose($ftmp);
    }


//    plot_map($tlon,$tlat);


page_tail();


function view_waveform_images_and_headers($f_out,$res_arr,$mag_last,$nt) {
/* This function handles the output of quake, sensor, and waveform information needed for the info window in google maps*/
   
  // echo "ie=".$nt."\n";
  for ($k = 0; $k<2; $k++) {
   $sensor_lons=array();
   $n_s=-1;
   $quake_lons=array();
   $n_q=-1;
   for ($i = 1; $i <= $nt; $i++) {
    if ($n_s==-1) {
     $n_s=0;
     $n_q=0; 
     if ($k==0) {
       $sensor_lons[$n_s]=$res_arr[$i]->trigger_lon;
     } else {
       $sensor_lons[$n_q]=$res_arr[$i]->quake_lon;
     }
     $n_ind_s[$n_s]=0;
     $ind_s[$n_s][$n_ind_s[i]]=1;
    } else {
     $i_s=$n_s+1;
     $i_q=$n_q+1;
     for ($j = 0; $j <= $n_s;$j++) {
      if ($k==0) {
        if ($sensor_lons[$j]==$res_arr[$i]->trigger_lon) {$i_s=$j;};
      } else {
        if ($sensor_lons[$j]==$res_arr[$i]->quake_lon) {$i_s=$j;};
      } 
     //echo "j=".$j." Lon=".$sensor_lons[$j]." Lons=".$res_arr[$i]->trigger_lon; 
     }
     if ($i_s>$n_s) {$n_s++;$n_ind_s[$i_s]=-1; $sensor_lons[$i_s]=$res_arr[$i]->trigger_lon;}

     $n_ind_s[$i_s]++;
     $ind_s[$i_s][(int)$n_ind_s[$i_s]]=$i;
     //echo "n_s".$n_s." i_s=".$i_s." n_ind_s[i_s]=".$n_ind_s[$i_s]."<br>\n";

    } 
   }

   for ($i=0;$i<=$n_s;$i++){    
    $message = "";
    for ($j=0;$j<=$n_ind_s[$i];$j++) {
     $res = $res_arr[(int)$ind_s[$i][$j]];
     $file_in  = get_file_url($res_arr[(int)$ind_s[$i][$j]]);
     if (preg_match("/N/i","substr($file,0,1)")) {

     } else {
  //   echo $file_in. "\n";
     if ($j==0) {$message  = view_waveform_sensor_header($res_arr[(int)$ind_s[$i][$j]],false);}
   //  echo "j=".$j."  ind_s=".(int)$ind_s[$i][$j];
     $message .= view_waveform_quake_header($res_arr[(int)$ind_s[$i][$j]],true);
     $message .= view_waveform_image($file_in);
    }
}
 //   echo $message;
    if ($k == 0) {
    fprintf($f_out,"%f;%f;%s;%f;%s\n",$res_arr[(int)$ind_s[$i][0]]->trigger_lon,$res_arr[(int)$ind_s[$i][0]]->trigger_lat,$message,4,$res_arr[(int)$ind_s[$i][0]]->sensor_description);
    } else {
    if (substr($res_arr[(int)$ind_s[$i][0]]->description,0,3)=="QCN") { $typeE = "QCN"; } else { $typeE = "USGS"; }
    fprintf($f_out,"%f;%f;%s;%f;%s\n",$res_arr[(int)$ind_s[$i][0]]->quake_lon,$res_arr[(int)$ind_s[$i][0]]->quake_lat,$message,round($res_arr[(int)$ind_s[$i][0]]->quake_magnitude,2),$typeE);
   }    
    }

   }    
/*   for ($i=0;$i<=$n_q;$i++){    
    $message = "";
    for ($j=0;$j<=$n_ind_q;$j++) {
     $res=$res_arr[(int)$ind_q[$j]];
     $file_in  = get_file_url($res);
     if ($j==0) {$message = view_waveform_quake_header($res,false);
     }
     $message .= view_waveform_sensor_header($res,true);
     $message .= view_waveform_image($file_in);
    }
*/
   return;
}

function view_waveform_image_and_header($f_out,$res,$mag_last) {
/* This function handles the output of quake, sensor, and waveform information needed for the info window in google maps*/
   $file_in  = get_file_url($res);
   //echo $file_in."";
   $message = "";

   $message  = view_waveform_quake_header($res);
   $message .= view_waveform_sensor_header($res);
   $message .= view_waveform_image($file_in);
   fprintf($f_out,"%f;%f;%s;%f;%s\n",$res->trigger_lon,$res->trigger_lat,$message,round($res->quake_magnitude,2),$res->sensor_description);
           
/*   if ($mag_last!=round($res->quake_magnitude,2)) {
     $mag_last =round($res->quake_magnitude,2);
     $message  = view_waveform_quake_header($res);
     $message .= view_waveform_image($file_in);
     if (substr($res->description,0,3)=="QCN") { $typeE = "QCN"; } else { $typeE = "USGS"; }
     fprintf($f_out,"%f;%f;%s;%f;%s\n",$res->quake_lon,$res->quake_lat,$message,round($res->quake_magnitude,2),$typeE);            
   }
*/
   return;
}

function view_waveform_quake_header($res,$print_dist=null) {
/* This function handles the output of the quake information needed for the info window in google maps*/
   $message  = "<font size=\"-3\"><br><b>Quake</b>: ".str_replace("","",$res->description);
   $message .= "<b>Lon</b>: ".round($res->quake_lon,2).", <b>Lat</b>: ".round($res->quake_lat,2)."";
   $message .= ", <b>Mag</b>: ".round($res->quake_magnitude,1)."<br>";
   $message .= "<b>Time</b>:".time_str($res->quake_time)."<br>";
//   $message .= "<b>Reported By</b>:";
//    if (substr($res->description,echo substr($res->description,0,3);
//   if ($print_dist) {$message .= "<br><b>Distance</b>:".round($res->quake_distance_km,2);}
//   if (substr($res->description,0,3)=="QCN") { $message .= "QCN</ul>"; } else { $message .= "USGS</font>"; }
   return $message;
}

function view_waveform_sensor_header($res,$print_dist=null) {
/* This function handles the output of the sensor information needed for the info window in google maps*/
   $message = "<font size=\"-3\"></b><p><b>Sensor</b>: ".$res->hostid;// <a href=\"". BASEURL ."/sensor/show_host_detail.php?hostid=".$res->hostid."\">".$res->hostid."</a>";
   $message .= ", <b>Lon</b>: ".round($res->trigger_lon,2).", <b>Lat</b>: ".round($res->trigger_lat,2)."";
   $message .= ", <b>Type</b>:".$res->sensor_description;
   if ($print_dist) {$message.="<br><b>Distance</b>:".round($res->quake_distance_km,2);}
   $message .="</font>";
   return $message;
}

function view_waveform_image($file_in) {
/* This function handles the output of the waveform information needed for the info window in google maps*/
   $message = "<p><iframe src=\"" . BASEURL . "/earthquakes/view/view_data.php?dat=".basename($file_in)."&fthumb=250\" frameborder=\"0\" scrolling=\"auto\"></iframe>";
   return $message;
}



function qcn_trigger_header_csv($auth) {
   $value = "TriggerID, HostID, ";
   if ($auth) {
    $value = $value."IPAddr(Hostname), ";
   }
   $value = $value ."ResultName, ";
   $value = $value 
    . "TimeTrigger, Delay, TimeSync, SyncOffset, "
    . "Magnitude, Significance, Latitude, Longitude, Resets, DT, Sensor, Version, GeoIP, Time File Req, "
    . "Received File, File Download, Quake ID, Quake Dist (km), Quake Magnitude, Quake Time, "
    . "Quake Lat, Quake Long, USGS GUID, Quake Desc, Is Archive?"
    . "\n";

   return $value;
}

function qcn_trigger_detail_csv(&$res,$auth,$user)
{
    $quakestuff = ",,,,,,,,";
    if ($res->qcn_quakeid) {
          $quakestuff = $res->qcn_quakeid . "," .
             $res->quake_distance_km . "," .
             $res->quake_magnitude . "," . 
             time_str_csv($res->quake_time) . "," .
             $res->quake_lat . "," .
             $res->quake_lon . ",\"" .
             $res->guid . "\",\"" .
             $res->description . "\",";
    }

   $file_url = get_file_url($res);

   if ($auth || $user->id == $res->hostid) {
    $loc_res = 4;
   } else {
    $loc_res = 2;
   }

   $value = $res->triggerid . "," . $res->hostid . ",";
   if ($auth) {
    $value = $value . $res->ipaddr . " (" . $res->hostname . "),";
   }
   $value = $value .
       $res->result_name . "," . time_str_csv($res->trigger_time) . "," . round($res->delay_time, 2) . "," .
        time_str_csv($res->trigger_sync) . "," . $res->sync_offset . "," . $res->trigger_mag . "," . $res->significance . "," .
        round($res->trigger_lat, $loc_res) . "," . round($res->trigger_lon, $loc_res) . "," . ($res->numreset ? $res->numreset : 0) . "," .
        $res->delta_t . "," . $res->sensor_description . "," . $res->sw_version . "," . $res->is_geoip . "," .
        time_str_csv($res->trigger_time) . "," . ($res->received_file == 100 ? " Yes " : " No " ) . "," .
        $file_url . "," .
        $quakestuff .
         ($res->is_archive ? "Y" : "N" ) . 
        "\n";

    return $value;

}

function qcn_trigger_header($auth) {
    echo "
        <tr>\n";
   if ($auth) {
    echo "
        <th><font size=\"1\">Request<BR>Files?</font size></th>
        <th><font size=\"1\">Batch<BR>Download?</font size></th>\n";
   }
    echo "
        <th><font size=\"1\">Trigger ID</font size></th>
        <th><font size=\"1\">Host ID</font size></th>";
   if ($auth) {
    echo "<th><font size=\"1\">IP Addr/Hostname</font size></th>";
   }
//        <th><font size=\"1\">Result</font size></th>
    echo "
        <th><font size=\"1\">Trigger Time</font size></th>
        <th><font size=\"1\">Time Delay (s)</font size></th>
        <th><font size=\"1\">Time Sync</font size></th>
        <th><font size=\"1\">Sync Offset(s)</font size></th>
        <th><font size=\"1\">PGA|<b>a</b>|<sub>0</sub> (m/s<sup>2</sup>)</font size></th>
        <th><font size=\"1\">Sig / Noise</font size></th>
        <th><font size=\"1\">Latitude</font size></th>
        <th><font size=\"1\">Longitude</font size></th>
        <th><font size=\"1\">Number Resets</font size></th>
        <th><font size=\"1\">dt (s)</font size></th>
        <th><font size=\"1\">Sensor</font size></th>
        <th><font size=\"1\">QCN V.</font size></th>
        <th><font size=\"1\">GeoIP?</font size></th>
        <th><font size=\"1\">Time File Req</font size></th>";
//   echo "        <th><font size=\"1\">File Received</font size></th>";
   echo "
        <th><font size=\"1\">File Download</font size></th>
        <th><font size=\"1\">View</font size></th>
        <th><font size=\"1\">Quake ID</font size></th>
        <th><font size=\"1\">Quake Dist (km)</font size></th>
        <th><font size=\"1\">Quake Magnitude</font size></th>
        <th><font size=\"1\">Quake Time (UTC)</font size></th>
        <th><font size=\"1\">Quake Latitude</font size></th>
        <th><font size=\"1\">Quake Longitude</font size></th>
        <th><font size=\"1\">Quake Description</font size></th>";
//   echo "<th><font size=\"1\">USGS GUID</font size></th>";
   echo "
        <th><font size=\"1\">Archived?</font size></th>
        </tr>
    ";
}


function qcn_trigger_detail(&$res,$bg_color,$auth,$user, $bDownloadAll) 
{
global $unixtimeArchive;
    if ($auth || $user->id == $res->hostid) {
      $loc_res = 4;
    } else {
      $loc_res = 2;
    }
  // CMC took out hostnamebyid below
    $sensor_type = $res->sensor_description;
    $archpre = $res->is_archive ? "a" : "r"; // prefix to signify if it's an archive record or not
    $file_url = get_file_url($res);
    if ($auth) {
      if ($bDownloadAll) {
        echo "<input type=\"hidden\" name=\"cb_" . $archpre . "_dlfile[]\" id=\"cb_" . $archpre . "_dlfile[]\" value=\"$res->triggerid\"" .
             ">\n";
        //echo "<tr><td><input type=\"hidden\" name=\"cb_" . $archpre . "_dlfile[]\" id=\"cb_" . $archpre . "_dlfile[]\" value=\"$res->triggerid\"" .
        //     "></font size></td></tr>\n";
        return;
      }
      else {
      echo "
        <tr bgcolor=\"".$bg_color."\">\n";
      echo"
        <td><font size=\"1\"><input type=\"checkbox\" name=\"cb_" . $archpre . "_reqfile[]\" id=\"cb_" . $archpre . "_reqfile[]\" value=\"$res->triggerid\"" . 
       ($res->varietyid!=0 || $res->received_file == 100 || $res->trigger_timereq>0 || $res->trigger_time < $unixtimeArchive ? " disabled " : " " ) . 
       "></font size></td>
        <td><font size=\"1\"><input type=\"checkbox\" name=\"cb_" . $archpre . "_dlfile[]\" id=\"cb_" . $archpre . "_dlfile[]\" value=\"$res->triggerid\"" . 
       (($res->received_file != 100 || file_url == "N/A") ? " disabled " : ($bDownloadAll ? " checked " : " " )) . 
       "></font size></td>";
      }
    }

    echo "
        <td><font size=\"1\">$res->triggerid</font size></td>";
        echo "<td><font size=\"1\"><a href=\"show_host_detail.php?hostid=$res->hostid\">" . $res->hostid . "</a></font size></td>";
    if ($auth) {
     echo "   <td><font size=\"1\">$res->ipaddr<br>$res->hostname</font size></td>";
    }
//    echo "<td><font size=\"1\">$res->result_name</font size></td>";
    echo "
        <td><font size=\"1\">" . time_str($res->trigger_time) . "</font size></td>
        <td><font size=\"1\">" . round($res->delay_time, 2) . "</font size></td>
        <td><font size=\"1\">" . time_str($res->trigger_sync) . "</font size></td>
        <td><font size=\"1\">".round($res->sync_offset,2)."</font size></td>
        <td><font size=\"1\">".round($res->trigger_mag,2)."</font size></td>
        <td><font size=\"1\">".round($res->significance,2)."</font size></td>
        <td><font size=\"1\">" . round($res->trigger_lat,$loc_res) . "</font size></td>
        <td><font size=\"1\">" . round($res->trigger_lon,$loc_res) . "</font size></td>
        <td><font size=\"1\">" . ($res->numreset ? $res->numreset : 0) . "</font size></td>
        <td><font size=\"1\">$res->delta_t</font size></td>
        <td><font size=\"1\">$sensor_type</font size></td>
        <td><font size=\"1\">$res->sw_version</font size></td>
        <td><font size=\"1\">$res->is_geoip<font size></td>";
      
        echo "
        <td><font size=\"1\">" . time_str($res->trigger_timereq) . "</font size></td>";
//      echo"  <td><font size=\"1\">" . ($res->received_file == 100 ? " Yes " : " No " ) . "</font size></td>";
 
        if ($file_url != "N/A") {
          echo "<td><font size=\"1\"><a href=\"" . $file_url . "\">Download</a></font size></td>";
          echo "<td><font size=\"1\"><a href=\"javascript:void(0)\"onclick=\"window.open('" . BASEURL . "/earthquakes/view/view_data.php?dat=".basename($file_url)."&fthumb=340','linkname','height=550,width=400,scrollbars=no')\">View</a></font size></td>";
        }
        else {
          echo "<td><font size=\"1\">N/A</font size></td>";
          echo "<td><font size=\"1\">N/A</font size></td>";
        }

        if ($res->qcn_quakeid) {
           echo "<td><font size=\"1\"><A HREF=\"$res->quake_url\">$res->qcn_quakeid</A></font size></td>";
           echo "<td><font size=\"1\">" . round($res->quake_distance_km,2) . "</font size></td>";
           echo "<td><font size=\"1\">" . round($res->quake_magnitude,2) . "</font size></td>";
           echo "<td><font size=\"1\">" . time_str($res->quake_time) . "</font size></td>";
           echo "<td><font size=\"1\">" . round($res->quake_lat,$loc_res) . "</font size></td>";
           echo "<td><font size=\"1\">" . round($res->quake_lon,$loc_res) . "</font size></td>";
           echo "<td><font size=\"1\">$res->description</font size></td>";
//           echo "<td><font size=\"1\">$res->guid</font size></td>";
           echo "<td><font size=\"1\">" . ($res->is_archive ? "Y" : "N") . "</font size></td>";
        }
        else {
           echo "<td><font size=\"1\">N/A</font size></td>";
           echo "<td><font size=\"1\">&nbsp</font size></td>";
           echo "<td><font size=\"1\">&nbsp</font size></td>";
           echo "<td><font size=\"1\">&nbsp</font size></td>";
           echo "<td><font size=\"1\">&nbsp</font size></td>";
           echo "<td><font size=\"1\">&nbsp</font size></td>";
           echo "<td><font size=\"1\">&nbsp</font size></td>";
//           echo "<td><font size=\"1\">&nbsp</font size></td>";
           echo "<td><font size=\"1\">" . ($res->is_archive ? "Y" : "N") . "</font size></td>";
        }
    echo "</tr>
    ";
}

function query_count($myquery) {
        $count_query = "select count(*) as cnt from ( $myquery ) mydb ";
        $result = mysql_query($count_query);
        if (!$result) return 0;
        $res = mysql_fetch_object($result);
        mysql_free_result($result);
        return $res->cnt;
}

function time_str_csv($x) {
    if (!$x) return "";
    return gmdate('Y/m/d H:i:s', $x); // . " UTC";
}

function get_file_url($res)
{
global $db_name;
$fileurl = "N/A";
if ($res->received_file == 100) {
   if ($res->is_archive) {
     $fileurl = UPLOADURL . "/trigger/archive/";
   }
   else {
     $fileurl = UPLOADURL . "/trigger/";
   }
   if ($db_name == "continual") {
      $fileurl .= "continual/";
   }
   if ($res->is_archive) { // add the fanout dir
     $fandir = floor($res->trigger_time/10000);
     $fileurl .= $fandir . "/";
   }
   $fileurl .= $res->trigger_file;
}
return $fileurl;
}

function printDownloadOptions($bUseCSV, $ftmp, $fileTemp, $bResultShow, $bUseArchive, $db_name, $auth, $plot_map)
{

//echo "<BR>Options: $bUseCSV  $ftmp  $fileTemp  $bResultShow  $bUseARchive  $auth  $plot_map\n<BR><BR>";

if ($bUseCSV && $ftmp) {
  echo "<BR><BR><A HREF=\"" . $fileTemp . "\">Download CSV/Text File Here (File Size " . sprintf("%7.2f", (filesize($fileTemp) / 1e6)) . " MB)</A> (you may want to right-click to save locally)<BR><BR>";
}
else if ($bResultShow) {
 echo "<input type=\"hidden\" id=\"cbUseArchive\" name=\"cbUseArchive\" value=\"" . ($bUseArchive ? "1" : "") . "\"> \n";
 echo "<input type=\"hidden\" id=\"db_name\" name=\"db_name\" value=\"" . $db_name . "\">\n ";
 if ($auth && $plot_map!="y") {

 echo "
  <input type=\"button\" value=\"Check All File Requests\" onclick=\"SetAllCheckBoxes('formDetail', 'cb_a_reqfile[]', true); SetAllCheckBoxes('formDetail', 'cb_r_reqfile[]', true);\" >\n
  <input type=\"button\" value=\"Uncheck All File Requests\" onclick=\"SetAllCheckBoxes('formDetail', 'cb_a_reqfile[]', false); SetAllCheckBoxes('formDetail', 'cb_r_reqfile[]', false);\" >\n
  <BR><BR>\n
  <input type=\"button\" value=\"Check All Download Requests\" onclick=\"SetAllCheckBoxes('formDetail', 'cb_a_dlfile[]', true); SetAllCheckBoxes('formDetail', 'cb_r_dlfile[]', true);\" >\n
  <input type=\"button\" value=\"Uncheck All Download Requests\" onclick=\"SetAllCheckBoxes('formDetail', 'cb_a_dlfile[]', false); SetAllCheckBoxes('formDetail', 'cb_r_dlfile[]', false);\" >
  <BR><BR>\n
  <input type=\"submit\" id=\"submitAll\" name=\"submitAll\" value=\"Submit All Requests\" /> ";
  }
}
}

?>
