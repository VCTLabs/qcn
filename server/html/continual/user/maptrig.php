<?php

require_once('../inc/boinc_db.inc');
require_once('../inc/util.inc');
require_once('../inc/translation.inc');
require_once('../inc/phoogle.inc');
require_once('../project/project_specific_prefs.inc');

$cx = get_double("cx", true);
$cy = get_double("cy", true);
$zoom = get_int("zoom", true);
$mapwidth = get_int("width", true);
$mapheight = get_int("height", true);
$timeint = get_str("timeint", true);  // H / D / M / W - blank or H is 4-hourly, D = day, M=month, W = week

//see if hostid gets a value
$hostid = get_int("hostid", true);  // note the (int) so it's a safe sql statement i.e. no sql injection string
if ($hostid>0) {
  db_init();
  $sqlhost = "select latitude, longitude, round(latitude,2) lat, round(longitude,2) lng, ifnull(q.hostid, 0) from qcn_trigger t left outer join qcn_showhostlocation q ON t.hostid=q.hostid 
  where t.hostid=$hostid order by time_trigger desc limit 1";
  $result = mysql_query($sqlhost);
  if ($result && ($res = mysql_fetch_array($result))) {
      if ($res[4]) {
      $cx = $res[0];
      $cy = $res[1];
      }
      else { // round
      $cx = $res[2];
      $cy = $res[3];
      }
      mysql_free_result($result); 
  }  
  else { // no host info, set zoom level to default
      $zoom = 0;
  }
}

$title = "";

    $mapimg = MAP_TRIGGER;
    $cachedatafile = CACHE_PATH_MAPTRIG_H;
    $cachedatafileContinual = CACHE_PATH_MAPTRIG_CONTINUAL_H;
    $legendbase = "<p align=\"left\"><font size=\"+1\"><b>Legend:</b></font><table><tr><td valign=\"center\"><IMG SRC=\"img/qcn_32_laptop.png\" height=\"24\"> = Laptop Sensor</td><td valign=\"middle\"><IMG SRC=\"img/qcn_32_usb.png\" height=\"24\"> = USB sensor</td><td valign=\"middle\"> <IMG SRC=\"img/qcn_32_quake.png\" height=\"24\"> = USGS-earthquake, magnitude >";
    switch($timeint) {
       case "D":
          $mapimg = MAP_TRIGGER_D;
          $cachedatafile = CACHE_PATH_MAPTRIG_D;
          $cachedatafileContinual = CACHE_PATH_MAPTRIG_CONTINUAL_D;
          $title = "Map for the Last Day";
          $legend = $legendbase . MIN_MAGNITUDE_D;
          break;
       case "W":
          $mapimg = MAP_TRIGGER_W;
          $cachedatafile = CACHE_PATH_MAPTRIG_W;
          $cachedatafileContinual = CACHE_PATH_MAPTRIG_CONTINUAL_W;
          $title = "Map for the Last Week";
          $legend = $legendbase . MIN_MAGNITUDE_W;
          break;
       case "M":
          $mapimg = MAP_TRIGGER_M;
          $cachedatafile = CACHE_PATH_MAPTRIG_M;
          $cachedatafileContinual = CACHE_PATH_MAPTRIG_CONTINUAL_M;
          $title = "Map for the Last Month";
          $legend = $legendbase . MIN_MAGNITUDE_M;
          break;
       default:
          $mapimg = MAP_TRIGGER;
          $cachedatafile = CACHE_PATH_MAPTRIG_H;
          $cachedatafileContinual = CACHE_PATH_MAPTRIG_CONTINUAL_H;
          $title = "Map for the Last 4 Hours";
          $legend = $legendbase . MIN_MAGNITUDE;
          break;
    }

    $title .= " (Generated on " . date("F d Y H:i:s", filectime($cachedatafile)) . " UTC)";
    $legend .= "</td></tr></table>\n";
//page_head(tra($title), null, $title, "", true, null, true);
page_head(null, null, null, "", true, null, true);

echo "<h3>" . $title . "</h3>";
echo "<h5>" . $legend . "</h5>";
echo "<ul>\n";
echo "<li><b>Note:</b> locations accurate to ~1 km for privacy (unless participant authorized exact location)<BR>";
echo "<li><b>Navigation:</b></li><ul>\n";
echo "    <li><b>Move Map:</b> Click and drag map on empty region.</li>\n";
echo "    <li><b>Zoom in:</b> Double click on empty region</li>\n";
echo "    <li><b>Zoom out:</b> Left double click (or ctrl-dble-click)</li>\n</ul>\n";
echo "<li>Click <b>'Refresh'</b> on your browser if the map or sensors do not display.<BR>";

$pm = new PhoogleMap();

if ($zoom) {
   $pm->zoomLevel = $zoom;
}
else {
   $pm->zoomLevel = 3;
}

if ($mapwidth) {
   $pm->setWidth($mapwidth);
}
else {
   $pm->setWidth(1);
}

if ($mapheight) {
   $pm->setHeight($mapheight);
}
else {
   $pm->setHeight(1);
}

if ($cx && $cy) {
   $pm->centerMap($cx, $cy);
}
else {  // default to northern California
   $pm->centerMap(38, -100);
}

$data = array();

$bUseContinual = false;

if (file_exists($cachedatafile)) {
print $cachedatafile . "<BR><BR>";
  $cacheddata=get_cached_data(MAPTRIG_TTL, $cachedatafile, $cachedatafile);  // regenerate every 15 minutes
  if ($cacheddata){
          $data = unserialize($cacheddata); // use the cached data
  } 
}

// continual data
if (file_exists($cachedatafileContinual)) {
  $cacheddataContinual=get_cached_data(MAPTRIG_TTL, $cachedatafile, $cachedatafile);  // regenerate every 15 minutes
  if ($cacheddataContinual){
          $bUseContinual = true;
          $dataContinual = unserialize($cacheddataContinual); // use the cached data
  } 
}

$i = 0;
for ($i = 0; $i < sizeof($data); $i++) {
  if ($data[$i]) {
     $pm->addGeoPoint($data[$i]->lat, $data[$i]->lng, $data[$i]->descript, $data[$i]->is_quake, $data[$i]->is_usb);
  }
} 

if ($bUseContinual)  { // show continual points
  $i = 0;
  for ($i = 0; $i < sizeof($dataContinual); $i++) {
    if ($dataContinual[$i]) {
       $pm->addGeoPoint($dataContinual[$i]->lat, $dataContinual[$i]->lng, $dataContinual[$i]->descript, 
           $dataContinual[$i]->is_quake, $dataContinual[$i]->is_usb);
    }
  } 
}

$pm->showMap();

echo "<center><h5><a href=maptrig.php?timeint=H>Hour</a> - <a href=maptrig.php?timeint=D>Day</a> - <a href=maptrig.php?timeint=W>Week</a> - <a href=maptrig.php?timeint=M>Month</a>";

page_tail();

?>
