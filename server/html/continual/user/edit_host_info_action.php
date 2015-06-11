<?php


// the below function is quite similar to the validate_form from edit_host_info_form.php, but conforms to PHP rather than javascript
function validate_form_line($testlat, $testlng, $testip, $testlevel, &$errmsg)
{ // check ip addresses, lat & lng
     for ($i = 0; $i < 5; $i++) {
       if ($testlat < -90 || $testlat > 90) {
           $errmsg = "Latitude is not between -90 and 90! (Note: South of equator is negative)";
           return false;
       }
       if ($testlng < -180 || $testlng > 180) {
           $errmsg = "Longitude is not between -180 and 180! (Note: West of Greenwich is negative)";
           return false;
       }
       if (($testlat == "" && $testlng) || ($testlat && $testlng == "")) { // left one blank
           $errmsg = "Latitude/Longitude pair is invalid, i.e. one is blank";
           return false;
       }
       if ($testlevel < 0 || $testlevel > $_POST["txthidLEVEL"]) {
           $errmsg = "Incorrect level type chosen";
           return false;
       }

       // now test IP address of form A.B.C.D or A.B.C (we just want A.B.C)
       if ($testip) {
         $iparr = explode(".", $testip);
         $arrlen = count($iparr);
         if ($testip == "127.0.0.1" || $testip == "127.0.0") {
            $errmsg = "Loopback IP address is not usefull for a lookup, please use an external IP address.";
            return false;
         }
         if ($arrlen < 3 || $arrlen > 4) {
            $errmsg = "Invalid IP Address entered, should be form of W.X.Y.Z or X.Y.Z";
            return false;
         }   
         $bgood = true;
         for ($j = 0; $j < $arrlen; $j++) {
            if ($iparr[$j] < 1 || $iparr[$j] > 255) { $bgood = false; break; }
         }
         if (!$bgood) {
            $errmsg = "Invalid IP Address entered, each section should be between 1 and 255";
            return false;
         }

         // so if we're here, but they didn't put in a lat/lng, they may have put in a good iP address with no lat/lng!
         if ($testlat == "" || $testlng == "") { // left one blank
           $errmsg = "IP Address entered, but no lat/lng to associate with it.";
           return false;
         }
       }
     }
     return true;  // if made it to here we're OK
}

function qcn_host_edit_error_page($errheader, $errtext)
{
   page_head($errheader);
   echo "<BR>" . $errtext . "<BR><BR>";
   echo "<FORM><INPUT TYPE=\"button\" VALUE=\"Back\" onClick=\"history.go(-1);return true;\"> </FORM>";
   page_tail();
   exit();
}


require_once("../inc/db.inc");
require_once("../inc/util.inc");

db_init();

$user = get_logged_in_user();

//verify this logged in user owns this host!
$hostid = $_POST['txthidHOST'];
$host = BoincHost::lookup_id($hostid);

if (!$hostid || $host->userid != $user->id)
{
   qcn_host_edit_error_page("Invalid_Host", 
      "It appears that you do not own this host machine, therefore you cannot edit location preferences!"
   );
}


// form vars passed in are: txthidHOST, lnm[0-4], lat[0-4], lng[0-4], ipa[0-4]

// need to validate the above (we've already validated host by now)
// if passes validation delete rows in qcn_host_ipaddr where hostid=$hostid and geoipaddrid=0
// then insert rows from the above posted fields

// probably do basic validation a la the onsubmit validation on the form, just so somebody isn't trying to post via a script?

for ($i = 0; $i < 5; $i++)
{
   $errmsg = "";
   if (!validate_form_line($_POST["lat" . $i], $_POST["lng" . $i], $_POST["ipa" . $i], $_POST["lvlt" . $i], $errmsg)) {
      qcn_host_edit_error_page("Invalid Data Entry", $errmsg . "<BR><BR>Use the 'Back' button below or in your browser to check your settings/choices and try again.");
   }
}

$ir = $_POST['cboAlign'];
$iRadioAlign = 0;
// verify valid value for alignment i.e. unaligned, north, wall (0,1,5) from qcn_align table
if ($ir >= 0 && $ir <= 6) { //== 1 || $ir == 5) {
   $iRadioAlign = $ir;
}

// if we made it here we're doing OK, i.e. everything validated

// first delete the old records
$db = BoincDb::get();
$retval = $db->delete_aux("qcn_host_ipaddr", "hostid=" . $db->base_escape_string($host->id) . " AND geoipaddrid=0");
if (!$retval) { // error, just return
   qcn_host_edit_error_page("Database Error", "Error in deleting your old IP/Lat/Lng Records!<BR><BR>Use the 'Back' button below or in your browser to check your settings/choices and try again.<BR>");
}

// now insert each new line
// now insert each new line

for ($i = 0; $i < 5; $i++) {
  // a good line either has a lat or an ip addr (we've already validated a lat has a lng with it!)

  // first let's put IP address in form of just 3 first (we've already validated IP at this point)
  $ipaddr = $_POST["ipa" . $i];
  $iparr = explode(".", $ipaddr);
  if (count($iparr) == 4) { // still full IP address, just take first three tokens
     $ipaddr = $iparr[0] . "." . $iparr[1] . "." . $iparr[2];
  }

  if ($_POST["lat" . $i] || $_POST["ipa" . $i]) {
    $loc = $db->base_escape_string($_POST["lnm" . $i]);
    $lvlv = $db->base_escape_string($_POST["lvlv" . $i]);
    $lvlt = $db->base_escape_string($_POST["lvlt" . $i]);
    if ($lvlv == "" || $lvlt == "" || $lvlt == 0) {
       $lvlv = "NULL";
       $lvlt = "NULL";
    }
    $sql = "(hostid, ipaddr, location, latitude, longitude, levelvalue, levelid, alignid) "
       . " values (" . $host->id . ",'" . $db->base_escape_string($ipaddr) . "','" . $loc . "',"
       . $db->base_escape_string($_POST["lat" . $i]) . ", "
       . $db->base_escape_string($_POST["lng" . $i]) . ", "
       . $lvlv . ", " . $lvlt . ", " . $iRadioAlign . ")";

    //echo $sql;
    $retval = $db->insert("qcn_host_ipaddr", $sql);

    if (!$retval) { // error, just return
      //echo "<BR><BR>" . $sql . "<BR><BR>";
      qcn_host_edit_error_page("Database Error", "Error in inserting your new IP/Lat/Lng Records!<BR><BR>Use the 'Back' button below or in your browser to check your settings/choices and try again.<BR><BR>Note: if you get a 'Duplicate Entry' error - you most likely have entered locations<BR>with the IP addresses left blank (only one blank allowed) or with the same <BR>IP addresses (remember it's just the first 3 bytes/parts of the IP address - <BR>so 121.2.3.4 and 121.2.3.5 count as the same IP address for QCN).");
    }
  }
}

page_head($errheader);

// check the host map location info

$iMapCount = $_POST["txthidMAPEXACT"];
$bMapExact = $_POST["cbmapexact"];
$txtMsg = "";


// just update on a change i.e. went from 0 to 1 or 1 to 0
if ( ($bMapExact && !$iMapCount) ) { // adding an ID
   $retval = $db->do_query("insert into qcn_showhostlocation (hostid) values (" . $db->base_escape_string($hostid) . ")");
}
else if (!$bMapExact && $iMapCount)  { // removing an ID
   $retval = $db->do_query("delete from qcn_showhostlocation where hostid=" . $db->base_escape_string($hostid));
}

if ($bMapExact) {
   $txtMsg = "Your machine will be exactly shown on QCN participant maps and lists";
}
else {
   $txtMsg = "Your machine will <b>not</b> be exactly shown on QCN participant maps and lists, but will be shown to the nearest .01 latitude/longitude for privacy";
}

  // if made it here then we're fine
  echo "<BR>Your host machine location records for host id# $host->id / machine name <b>$host->domain_name</b> have been successfully updated!<BR>";
  echo "<BR>Thank you for helping us locate triggers from your machine.  Rest assured that all information will only be used for locating possible seismic events.";
  echo "<BR>IP addresses saved are only the first 3 bytes (i.e. the 153.2.3 part of 153.2.3.231)";
  if ($txtMsg) {
      echo "<BR><BR>" . $txtMsg . "<BR>";
  }
  page_tail();

?>
