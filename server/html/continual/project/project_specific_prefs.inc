<?php

require_once('../inc/cache.inc');

// Functions to display and edit project-specific prefs go here

// generate world map
function qcn_generate_world_map(&$data, $timeint)
{ // timeint can be H (default 4 hr), or D=Day, W=Week, M=Month
    $trigtime = 3600*4;
    $minmag = MIN_MAGNITUDE;
    $mapimg = MAP_TRIGGER;
    $cachedata = CACHE_PATH_MAPTRIG_H;
    $cachedataContinual = CACHE_PATH_MAPTRIG_CONTINUAL_H;
    switch($timeint) {
       case "D":
          $trigtime = 3600*24;
          $minmag = MIN_MAGNITUDE_D;
          $mapimg = MAP_TRIGGER_D;
          $cachedata = CACHE_PATH_MAPTRIG_D;
          $cachedataContinual = CACHE_PATH_MAPTRIG_CONTINUAL_D;
          break;
       case "W":
          $minmag = MIN_MAGNITUDE_W;
          $trigtime = 3600*24*7;
          $mapimg = MAP_TRIGGER_W;
          $cachedata = CACHE_PATH_MAPTRIG_W;
          $cachedataContinual = CACHE_PATH_MAPTRIG_CONTINUAL_W;
          break;
       case "M":
          $minmag = MIN_MAGNITUDE_M;
          $trigtime = 3600*24*31;
          $mapimg = MAP_TRIGGER_M;
          $cachedata = CACHE_PATH_MAPTRIG_M;
          $cachedataContinual = CACHE_PATH_MAPTRIG_CONTINUAL_M;
          break;
       default:
          $minmag = MIN_MAGNITUDE;
          $trigtime = 3600*4;
          $mapimg = MAP_TRIGGER;
          $cachedata = CACHE_PATH_MAPTRIG_H;
          $cachedataContinual = CACHE_PATH_MAPTRIG_CONTINUAL_H;
          break;
    }

    // get earthquake data first (shows up first)
    $strSQL =
      "select latitude lat, longitude lng, 0 is_usb, magnitude ctr, from_unixtime(time_utc) timetrig, 
         1 is_quake, concat('Magnitude: ', magnitude, '<BR>' , description, '<BR>Time: ', from_unixtime(time_utc), ' UTC') descript
           from qcn_quake
             where time_utc>unix_timestamp()-" . $trigtime . " and magnitude>=" . $minmag;
    $result = mysql_query($strSQL);
    $i = 0;
    if ($result) {
       while ($obj = mysql_fetch_object($result))
       {
         if ($obj && !($obj->lat == 0 && $obj->lng == 0)) { // make sure have a valid obj, and lat/lng is OK (i.e. not 0,0)
            $data[$i++] = $obj;
         }
       }
       mysql_free_result($result);
    }

    // get latest triggers per lat/lng point, with a random offset so we don't show their exact address
    $strSQL = 
      "select round(latitude, 2) lat, round(longitude, 2) lng, s.is_usb, count(*) ctr, from_unixtime(max(time_trigger)) timetrig, 
          0 is_quake, concat('Total: ', count(*), '<BR>Earliest: ', 
               from_unixtime(min(time_trigger)), ' UTC', '<BR>Latest: ', 
               from_unixtime(max(time_trigger)), ' UTC') descript
             from qcn_trigger t
              LEFT JOIN qcn_sensor s ON t.qcn_sensorid=s.id 
                where t.qcn_sensorid>0 AND t.hostid not in (select hostid from qcn_showhostlocation)
                 and time_trigger>unix_timestamp()-" . $trigtime . "
                   group by lat,lng,s.is_usb
         UNION
      select latitude lat, longitude lng, s.is_usb, count(*) ctr, from_unixtime(max(time_trigger)) timetrig, 
          0 is_quake, concat('Total (#', t.hostid, ' Location): ', count(*), '<BR>Earliest: ', 
               from_unixtime(min(time_trigger)), ' UTC', '<BR>Latest: ', 
               from_unixtime(max(time_trigger)), ' UTC') descript
             from qcn_trigger t
              LEFT JOIN qcn_sensor s ON t.qcn_sensorid=s.id 
                where t.qcn_sensorid>0 AND t.hostid in (select hostid from qcn_showhostlocation)
                 and time_trigger>unix_timestamp()-" . $trigtime . "
                   group by lat,lng,s.is_usb
       ";
    $result = mysql_query($strSQL);
    //mt_srand();  // seed the Mersenne random number generator
    if ($result) {
       while ($obj = mysql_fetch_object($result))
       {   
         if ($obj && !($obj->lat == 0 && $obj->lng == 0)) { // make sure have a valid obj, and lat/lng is OK (i.e. not 0,0)
            // use the Mersenne random number generator to add a factor to the lat/lng so we're not displaying exact addresses
            //$factor = (((mt_rand() / mt_getrandmax()) * 0.01) + (0.01 * (mt_rand() / mt_getrandmax()))) * ((mt_rand() % 2) ? -1.0 : 1.0);
            //$factor = ((mt_rand() / mt_getrandmax()) * 0.0099) * ((mt_rand() % 2) ? -1.0 : 1.0);
            //$obj->lat += $factor;
            //$factor = ((mt_rand() / mt_getrandmax()) * 0.0111) * ((mt_rand() % 2) ? -1.0 : 1.0);
            //$obj->lng += $factor;
            $data[$i++] = $obj;
         }
       }
       mysql_free_result($result);
    }

    // save the $data to the cache
    set_cache_data(serialize($data), "", $cachedata);

  // First we load the background/base map. We assume it's located in same dir as the script.
  // This can be any format but we are using JPG in this example
  // We will also allocate the color for the marker 

  $im = imagecreatefromjpeg(MAP_BASE);
  if (!$im) {
     echo "Error creating jpeg!<BR>";
     return;
  }

  $colorstation = imagecolorallocate ($im, 0, 0, 255);
  $colorquake   = imagecolorallocate ($im, 255, 0, 0);

  // Next need to find the base image size.
  // We need these variables to be able scale the long/lat coordinates. 

  $scale_x = imagesx($im);
  $scale_y = imagesy($im);

  // now print the participant trigger locations
  for ($i = 0; $i < sizeof($data); $i++) { // note $isave is max (+1) of the trigger vals before we added earthquake data
    if ($data[$i]) {
       // Now we convert the long/lat coordinates into screen coordinates 
       $x = (($data[$i]->lng + 180.0) * ($scale_x / 360.0));
       $y = ((($data[$i]->lat * -1.0) + 90.0) * ($scale_y / 180.0));
      
       if ($data[$i]->is_quake) { // quake - draw red circle
          // Now mark the point on the map using a circle
          imagefilledellipse($im, $x, $y, 6, 6, $colorquake);
       }
       else { // station
          // draw the station as blue triangles centered on x,y
          $pts = array( 
                 $x, $y-2,
                 $x-2, $y+2,
                 $x+2, $y+2
              );
          imagefilledpolygon($im, $pts, 3,$colorstation);
       }
    }
  }

  // Return the map image. We are using a PNG format as it gives better final image quality than a JPG 
  imagejpeg($im, $mapimg);
  imagedestroy($im);
}

function option($name, $val) {
    if ($name == $val) {
        $x = "selected";
    } else {
        $x = "";
    }
    return "<option name='$name' $x>$name\n";
}

function project_specific_prefs_default() {
    return "
        <ssp>0</ssp>
        <lat>-91</lat>
        <lng>-181</lng>
        <color_scheme>Tahiti Sunset</color_scheme>
        <max_frames_sec>100</max_frames_sec>
    ";
}

// given struct, show form for editing
// $error is a struct indicating which values were erroneous
// (value X is erroneous if $error->X is set)
//
function project_specific_prefs_edit($prefs, $error=false) {
    $x = $prefs->ssp;
    $y = "<select name=ssp>
            <option value=0 " . (($x==0) ? "selected" : "") . ">Default (Cycles Through All Views)
            <option value=1 " . (($x==1) ? "selected" : "") . ">3D Accelerometer Plot
            <option value=2 " . (($x==2) ? "selected" : "") . ">2D Accelerometer Plot
            <option value=3 " . (($x==3) ? "selected" : "") . ">Earthquake Globe (Day)
            <option value=4 " . (($x==4) ? "selected" : "") . ">Earthquake Globe (Night)
            <option value=6 " . (($x==6) ? "selected" : "") . ">Bouncing Cube w/ Plots
        </select>
    ";
    row2("Screensaver Preference", $y);

    if ($prefs->lat == -91) $prefs->lat = "";
    if ($prefs->lng == -181)  $prefs->lng = "";

    row2("Preferred Latitude [range: -90 (S), 90 (N)]", "<input size=9 name=lat value=\"" . $prefs->lat . "\">");
    row2("Preferred Longitude [range: -180 (W), 180 (E)]", "<input size=9 name=lng value=\"" . $prefs->lng . "\">");

    $x = $prefs->color_scheme;
    $y = "<select name=color_scheme>
            ".option("Tahiti Sunset", $x)
            .option("Desert Sands", $x)."
        </select>
    ";
    row2("Color scheme", $y);

    $y = "<input size=5 name=max_frames_sec value='$prefs->max_frames_sec'>";
    row2("Max frames/sec", $y, isset($error->max_frames_sec));
}

// Parse form vars, return XML version of project-specific prefs
// Also set $error, store the error values (see above)
//
function project_specific_prefs_parse_form(&$error) {
    $color_scheme = $_GET["color_scheme"];
    $ssp = get_int("ssp");
    $max_frames_sec = $_GET["max_frames_sec"];
    $lat = $_GET["lat"];
    $lng = $_GET["lng"];
    if (!verify_numeric($lat, -90.0, 90.0) || !verify_numeric($lng, -180.0, 180)) {
       $lat = '';
       $lng = '';
    }

    // add and modify this line for other user
    // editable values that should be validated
    //
    if (!verify_numeric($max_frames_sec, 0)) $error->max_frames_sec = true;

    // Please look at util.inc for further information regarding:
    // function verify_numeric(&$value, $low, $high = false)

    return "<color_scheme>$color_scheme</color_scheme>
        <ssp>$ssp</ssp>
        <lat>$lat</lat>
        <lng>$lng</lng>
        <max_frames_sec>$max_frames_sec</max_frames_sec>
    ";
}

// show non-editable version of prefs
//
function project_specific_prefs_show($prefs, $columns=false) {
    // Please add your prefs-values for both views!!
    //
    $y = ""; 
      switch($prefs->ssp) {
         case 0: $y = "Default (Cycles Through All Views)"; break;
         case 1: $y = "3D Accelerometer Plot"; break;
         case 2: $y = "2D Accelerometer Plot"; break;
         case 3: $y = "Earthquake Globe (Day)"; break;
         case 4: $y = "Earthquake Globe (Night)"; break;
         case 6: $y = "Bouncing Cube w/ Plots"; break;
      }
    if ($columns) {
        // This is used if columns-view is enabled
        row_defs("Screensaver Preference","ssp", "", "", $prefs);
        row_defs("Preferred Latitude","lat", "", "", $prefs);
        row_defs("Preferred Longitude","lng", "", "", $prefs);
        row_defs("Color scheme","color_scheme", "", "", $prefs);
        row_defs("Max frames/sec", "max_frames_sec", "", "", $prefs);
    } else {
        // This is used if normal-view is enabled
        row2("Screensaver Preference", $y);
        row2("Preferred Latitude", $prefs->lat);
        row2("Preferred Longitude", $prefs->lng);
        row2("Color scheme", $prefs->color_scheme);
        row2("Max frames/sec", $prefs->max_frames_sec);
    }
}

// parse XML, fill in struct
//
function project_specific_prefs_parse($prefs_xml) {
    $prefs = new stdClass();
    $prefs->ssp = parse_element($prefs_xml, "<ssp>");
    $prefs->lat = parse_element($prefs_xml, "<lat>");
    $prefs->lng = parse_element($prefs_xml, "<lng>");
    $prefs->color_scheme = parse_element($prefs_xml, "<color_scheme>");
    $prefs->max_frames_sec = parse_element($prefs_xml, "<max_frames_sec>");
    return $prefs;
}

function project_specific_host_latlng_prefs($hostid) {
    // lookup hostid and get the hostlatlng for this
    $hll = null;
    $hostid = (int) $hostid;
    if ($hostid < 1) return $hll; // invalid hostid (or sql injection attack?)
    $db = BoincDb::get();
    $sql = "select geoipaddrid,ipaddr,location,latitude,longitude,levelvalue,levelid,alignid from qcn_host_ipaddr "
           . "where hostid=$hostid order by geoipaddrid,ipaddr,id";
    $result=$db->do_query($sql);
    if ($result && mysql_num_rows($result)>0) {  // we found some records
       while($row=mysql_fetch_array($result)) {
         $hll[] = $row;
       }
    }
    return $hll;
}

?>
