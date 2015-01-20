<?php
chdir("/var/www//boinc/sensor/html/user");
require_once("/var/www//boinc/sensor/html/inc/earthquake_email.inc");
$mag  = 3.8; 
$longitude = 121.674; 
$latitude = 24.398; 
$depth = 41.8; 
$n_stations = 8; 
$etime = 1420606119.193318; 
$dtime = 1420606209; 
$dt_detect  = 89.8; 
$edir       = 1420606119; 

earthquake_email($mag,$longitude,$latitude,$depth,$n_stations,$etime,$edir,$dtime,$dt_detect);

?>
