<?php

require_once("../inc/db.inc");
require_once("../project/project.inc");
require_once("../inc/util.inc");
require_once("../inc/user.inc");
require_once("../inc/host.inc");


function lookup_host($id) {
    return BoincHost::lookup_id($id);
}

db_init();
$hostid = get_int("hostid");
$host = lookup_host($hostid);
if (!$host) {
    echo "Couldn't find computer";
    exit();
}
$user = get_logged_in_user(false);
if ($user && $user->id == $host->userid) {
    $private = false;
} else {
    $user = null;
    $private = true;
}

$heading = "Computer Triggers for Host # " . $host->id; //  . " (" . $host->domain_name . ")";
page_head($heading);
show_trigger($host->id, $heading, $private);
page_tail();

?>
