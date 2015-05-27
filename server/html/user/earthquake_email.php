<?php
chdir("/var/www//boinc/sensor/html/user");
require_once("/var/www//boinc/sensor/html/inc/earthquake_email.inc");
$mag  = 2.4; 
$longitude = -122.046; 
$latitude = 37.947; 
$depth = 11.8; 
$n_stations = 9; 
$etime = 1430691201.139523; 
$dtime = 1430691326; 
$dt_detect  = 124.9; 
$edir       = 1430691201; 

earthquake_email($mag,$longitude,$latitude,$depth,$n_stations,$etime,$edir,$dtime,$dt_detect);

?>
