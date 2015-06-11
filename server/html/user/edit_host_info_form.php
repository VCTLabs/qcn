<?php

ini_set('display_errors', 0);
require_once("../project/project.inc");
require_once("../inc/prefs.inc");
require_once("../inc/db.inc");
require_once("../inc/util.inc");
require_once("../inc/utils.inc");


db_init();

$user = get_logged_in_user();
$psprefs = project_specific_prefs_parse($user->project_prefs);

$db = BoincDb::get();

//verify this logged in user owns this host!
$hostid = get_int("hostid", true);
$bMapExact = 0;

if ($hostid) {
   $host = BoincHost::lookup_id($hostid);
}
else { // get the first host record for this user i.e. they may be a new user
  $sql = "SELECT id FROM host WHERE userid=" . $db->base_escape_string($user->id) . " ORDER BY id DESC LIMIT 1";
  $result=$db->do_query($sql);
  if ($result && mysql_num_rows($result)>0) {  // we found some records
     if ($row=mysql_fetch_array($result)) {
       $hostid = $row[0];
     }
  }
  if ($hostid) $host = BoincHost::lookup_id($hostid);
}

if (!$hostid || !$host || $host->userid != $user->id)
{
   page_head("Invalid Host");

   echo "<BR>It appears that you do not own this host machine, therefore you cannot edit location preferences!<BR>";
   echo "<BR><BR>If this is a new account, you may need to wait until BOINC connects and creates your Computer ID #.<BR>";
   echo "<BR>If this is the case, wait until BOINC is finished attaching to the project, then click 'Refresh' on this page.<BR>";
   page_tail();
   exit();
}

// first off get the sensor types
$sqlsensor = "select id,description from qcn_level order by id";
$result = mysql_query($sqlsensor);
$i = 0;
$arrLevel = array();
if ($result) {
    while ($res = mysql_fetch_array($result)) {
       $arrLevel[$i] = $res;
       $i++;
    }
    mysql_free_result($result);
}
$clvlmax = sizeof($arrLevel) - 1;

// get host qcn_showhostlocation info
$result = $db->do_query("select count(hostid) from qcn_showhostlocation where hostid=" . $db->base_escape_string($hostid));
  if ($result && mysql_num_rows($result)>0) {  // we found some records
     if ($row=mysql_fetch_array($result)) {
       $bMapExact = $row[0];
     }
  }

// get array of geoipaddrid,ipaddr,location,latitude,longitude
$hll = project_specific_host_latlng_prefs($host->id);

$startlat = "";
$startlng = "";
$bnewuser = false;

// hll is a 2-d array of rows for this host:
$hllsize = $hll ? count($hll) : 0;
$buserset = false;
$iRadioAlign = 0;

for ($i = 0; $i < $hllsize; $i++)
{
   if ($hll[$i][0] == 0) { $buserset=true; break; }  
}

// if $hll is empty, put in their current location via geoip
if (!$hll || !$buserset)
{ // no user set settings, so get ip lookup from their current address
  $ipaddr = ""; //$_SERVER['REMOTE_ADDR'];
  $iparr = explode(".", $ipaddr);
  if (count($iparr)==4) {
     $ipaddr = $iparr[0] . "." . $iparr[1] . "." . $iparr[2]; 
  }

  // now ipaddr should be just the first 3, lookup in our table
  $sql = "SELECT latitude,longitude FROM qcn_geo_ipaddr WHERE ipaddr='" . $db->base_escape_string($ipaddr) . "'";
  $result=$db->do_query($sql);
  if ($result && mysql_num_rows($result)>0) {  // we found some records
     // seed the startlat & startlng
     if ($row=mysql_fetch_array($result)) {
       $startlat = $row[0];
       $startlng = $row[1];
     }
  }
  else { // I suppose we should do a geoip lookup and insert into geoip table?
     $url = MAXMIND_URL . "&i=" . $ipaddr . ".1";
     $html = disguise_curl($url);
     if ($html) { // see if we got a good html response from maxmind for this ipaddr
       $iparr = explode(",", $html);
       if (count($iparr) == 5) { // example line:
          // US,CA,Stanford,37.417801,-122.171997
          $country  = $db->base_escape_string($iparr[0]);
          $region   = $db->base_escape_string($iparr[1]);
          $city     = $db->base_escape_string($iparr[2]);
          $startlat = $db->base_escape_string($iparr[3]);
          $startlng = $db->base_escape_string($iparr[4]);

         $sql = "(ipaddr, time_lookup, country, region, city, latitude, longitude) "
            . " values ('" . $ipaddr . "',unix_timestamp(),"
              . "'" . $country . "', "
              . "'" . $region . "', "
              . "'" . $city . "', "
              . $startlat . ", "
              . $startlng . ")";

         $retval = $db->insert("qcn_geo_ipaddr", $sql);
       }
     } 
  }

   // need to push this into the start of the hll array, i.e. may have geoip entries?
   // select geoipaddrid,ipaddr,location,latitude,longitude,levelvalue,levelid,alignid from qcn_host_ipaddr where hostid=$hostid order by geoipaddrid,location";

   $hll[$hllsize][0] = 0;
   $hll[$hllsize][1] = $ipaddr;
   $hll[$hllsize][2] = "";
   $hll[$hllsize][3] = $startlat;
   $hll[$hllsize][4] = $startlng;
   $hll[$hllsize][5] = "";
   $hll[$hllsize][6] = 0;
   $hll[$hllsize][7] = 0;
   $hllsize++; // increment size of hll
   $bnewuser = true;
}

$iRadioAlign = $hll[0][7];  // the first record is the radio btn alignment
$title = "Enter Up To Five Locations for Your Computer ID # " . $host->id . " Named: " . $host->domain_name;
page_head($title, null, null, "", true, $psprefs);

echo "
 <script type=\"text/javascript\">
  function validate_form(thisForm)
  {// check ip addresses, lat & lng
     for (i = 0; i < 5; i++) {
       var testlat  = getElement(\"lat\" + i).value;
       var testlng  = getElement(\"lng\" + i).value;
       var testip   = getElement(\"ipa\" + i).value;

       if (testlat < -90 || testlat > 90) {
           alert(\"Line #\" + (i+1) + \":  \" + testlat + \",  \" + testlng + \":\\nLatitude must be between -90 and 90\\n(Note: South of Equator is negative)\");
           return false;
       }
       if (testlng < -180 || testlng > 180) {
           alert(\"Line #\" + (i+1) + \":  \" + testlat + \",  \" + testlng + \":\\nLatitude must be between -180 and 180\\n(Note: West of Greenwich is negative)\");
           return false;
       }
       if ((testlat == \"\" && testlng) || (testlat && testlng == \"\")) { // left one blank
           alert(\"Line #\" + (i+1) + \":  \" + testlat + \",  \" + testlng + \":\\nYou must fill in both Latitude and Longitude (or leave both blank)\");
           return false;
       }
       // now test IP address of form A.B.C.D or A.B.C (we just want A.B.C)
       if (testip) {
         var iparr = testip.split(\".\");
         var arrlen = iparr.length;
         if (testip == \"127.0.0.1\" || testip == \"127.0.0\") {
            alert(\"Line #\" + (i+1) + \":  Loopback addresses are not useful for a lat/lng lookup; external IP address needed\");
            return false;
         }
         if (arrlen < 3 || arrlen > 4) {
            alert(\"Line #\" + (i+1) + \":  Invalid IP Address - should be formatted as X.Y.Z or W.X.Y.Z\");
            return false;
         }   
         var bgood = true;
         for (j = 0; j < arrlen; j++) {
            if (iparr[j] < 1 || iparr[j] > 255) { bgood = false; break; }
         }
         if (!bgood) {
            alert(\"Line #\" + (i+1) + \":  Invalid IP Address - each section should be between 1 and 255\");
            return false;
         }

         // so if we're here, but they didn't put in a lat/lng, they may have put in a good iP address with no lat/lng!
         if (testlat == \"\" || testlng == \"\") { // left one blank
           alert(\"Line #\" + (i+1) + \":  IP Address valid, but no Latitude and Longitude entered!\");
           return false;
         }
       }
     }
     return true;  // if made it to here we're OK
  }

</script>
";

echo "<form method=post action=edit_host_info_action.php onsubmit=\"return validate_form(this)\">";

$COLSPAN = "colspan=9";



start_table();

echo "<tr><td ><div id=\"map\" style=\"width: 600px; height: 600px\"></div><p> </p></td>\n";


echo "<td valign=\"top\"><font size=\"+1\"><b>Setting your sensors location:</b></font>\n";
echo "<ol>\n";
echo "<li><b>Use the following box to lookup an address:</b>\n";
echo "    <br><input type=\"text\" name=\"addrlookup\" id=\"addrlookup\" size=50 value=\"\"> <input type=\"button\" name=\"btnaddress\" id=\"btnaddress\" onclick=\"clickedAddressLookup(addrlookup.value)\" value=\"Lookup Address\" size=20>(i.e. 360 Panama Mall, Stanford, CA)\n";

echo "<li><b>Zoom in & Relocate:</b> with the Google map, please be as accurate as you can. The better we know your location the better we can pinpoint earthquakes!<BR>\n";

echo "<li><b>Link to IP Address (Optional):</b></li>\n";
echo "<ul><li><b>Single Location (USB Sensor):</b> Either leave the IP address blank (below) or hit <font color=\"red\">Clear Net Addr</font> (below) if this is the only location for this computer.  The computer will always be mapped to this location.</li>\n";

echo "<li><b>Laptops with Multiple Locations (optional):</b><br>*Enter an IP (network) address to associate with. This will map the computer to this location when your computer is on the same network.\n";

echo "    <br>*For each additional location your laptop resides, use a different color marker (below) and repeat step 3 (above) to mark that spot.\n";
echo "<br>*We only store the first 3 bytes of your IP address, and never store any address information.  All information is used solely to track seismic events for your area and will not be sold or shared with any other party.</li>\n";
echo "</ul>\n";
echo "<li><b>Enter Elevation/height/floor level:</b> You can choose the type of info with the <font color=\"\">Level Type</font> option below.  Make sure you use the correct units.</li>";

echo "<li><b>Share detailed location (optional):</b> You can share your sensor location openly online through maps by selecting this option below\n";
echo "<li><b>Designate Sensor Orientation: Please indicate if the sensor is oriented to north or not.</b>";
echo "<li><b>Update Info:</b> Your info will not be saved until you hit the <font color=\"red\">update Info</font> button below.</li>";echo "</ul>\n";
echo "</td></tr>\n";
end_table();
start_table();

if ($bnewuser) {
  echo "\n<tr><td $COLSPAN><font color=red>Note that the first row is set based on a guess based on your current IP address, this is not saved until you confirm by pressing the 'Update Info' button</font></td</tr>\n";
}
 
  echo "\n<tr><td $COLSPAN><font color=red><i>" .
      "<B>Either leave the IP address blank (below) or hit <font color=\"red\">Clear Net Addr</font> (below) if this is the only location for this computer.</B>" .
      "</i></font></td</tr>\n";

echo "\n<tr><th width=\"5\">Select</th><th>Location Name (optional)</th><th>Latitude</th><th>Longitude</th><th>Level (Height)</th><th>Level Type</th><th>Net (IP) Addr</th><th>Set Net  Addr</th><th>Clear Net Addr</th></tr>\n";

for ($i=0; $i<5; $i++)
{

   echo "<tr>
            <td width=\"5\"><img src=\"" . constant("MAP_ICON_" . $i) . "\">\n";
   echo "   <input type=\"radio\" name=\"radioSelect\" id=\"radioSelect\" value=\"" . $i . "\" onclick=\"clickedRadio("; 
   echo $i . ", this.value==" . $i . ")\" ";

   if ($i == 0) echo "checked";
   echo " ></td>";

   if ($i < $hllsize && $hll[$i][0] == 0) {
     // geoipaddrid,ipaddr,location,latitude,longitude
     echo "
            <td width=\"20\"><input type=\"text\" name=\"lnm" . $i . "\" id=\"lnm" . $i . "\" size=20 value=\"" . stripslashes($hll[$i][2]) . "\"></td>
            <td width=\"10\"><input type=\"text\" name=\"lat" . $i . "\" id=\"lat" . $i . "\" size=10 value=\"" . $hll[$i][3] . "\">
            </td>
            <td width=\"10\"><input type=\"text\" name=\"lng" . $i . "\" id=\"lng" . $i . "\" size=10 value=\"" . $hll[$i][4] . "\">
            </td>
            <td width=\"10\"><input type=\"text\" name=\"lvlv" . $i . "\" id=\"lvlv" . $i . "\" size=10 value=\"" . $hll[$i][5] . "\">
            </td>";

       echo "
            <td width=\"20\"><select name=\"lvlt" . $i . "\" id=\"lvlt" . $i . "\" >";

         for ($jj = 0; $jj <= $clvlmax; $jj++)  {
            echo "<option value=" . $arrLevel[$jj][0];
            if ($hll[$i][6] == $arrLevel[$jj][0]) echo " selected";
            echo ">" . $arrLevel[$jj][1] . "\n";
          }

          echo "</select>
            </td>
            <td width=\"15\"><input type=\"text\" name=\"ipa" . $i . "\" id=\"ipa" . $i . "\" size=13 value=\"" . $hll[$i][1] . "\">
            </td>
          ";
   }
   else {
     echo "
            <td width=\"20\"><input type=\"text\" name=\"lnm" . $i . "\" id=\"lnm" . $i . "\" size=20 value=\"\"></td>
            <td width=\"10\"><input type=\"text\" name=\"lat" . $i . "\" id=\"lat" . $i . "\" size=10 value=\"\">
            </td>
            <td width=\"10\"><input type=\"text\" name=\"lng" . $i . "\" id=\"lng" . $i . "\" size=10 value=\"\">
            </td>
            <td width=\"10\"><input type=\"text\" name=\"lvlv" . $i . "\" id=\"lvlv" . $i . "\" size=10 value=\"\">
            </td>";

       echo "
            <td width=\"20\"><select name=\"lvlt" . $i . "\" id=\"lvlt" . $i . "\" >";

         for ($jj = 0; $jj <= $clvlmax; $jj++)  {
            echo "<option value=" . $arrLevel[$jj][0];
            //if ($hll[$i][5] == $arrLevel[$jj][0]) echo " selected";
            echo ">" . $arrLevel[$jj][1] . "\n";
          }

          echo "</select></td>
            <td width=\"15\"><input type=\"text\" name=\"ipa" . $i . "\" id=\"ipa" . $i . "\" size=13 value=\"\">
            </td>
          ";
   }
/*  hidden fields if needed
              <input type=\"hidden\" name=\"hlat" . $i . "\" id=\"hlat" . $i . "\" value=\"" . $hll[$i][3] . "\">
              <input type=\"hidden\" name=\"hlng" . $i . "\" id=\"hlng" . $i . "\" value=\"" . $hll[$i][4] . "\">
               <input type=\"hidden\" name=\"hlat" . $i . "\" id=\"hlat" . $i . "\">
               <input type=\"hidden\" name=\"hlng" . $i . "\" id=\"hlng" . $i . "\">
             // <input type=\"hidden\" name=\"hipa" . $i . "\" id=\"hipa" . $i . "\" value=\"" . $hll[$i][1] . "\">
               <input type=\"hidden\" name=\"hipa" . $i . "\" id=\"hipa" . $i . "\">
*/

   echo "
            <td width=\"20\"><input type=\"button\" name=\"bipa" . $i . "\" id=\"bipa" . $i . "\" onclick=\"clickedButton(" . $i . ", false)\" value=\"Set Current\" size=10></td>
            <td width=\"20\"><input type=\"button\" name=\"bipac" . $i . "\" id=\"bipac" . $i . "\" onclick=\"clickedButton(" . $i . ", true)\" value=\"Clear\" size=10></td>
      </tr>
   ";

}
end_table();
start_table();
// checkbox for allowing location to be shown exactly on the map
echo "<tr><td colspan=2><BR><input type=\"checkbox\" id=\"cbmapexact\" name=\"cbmapexact\" ";
if ($bMapExact) {
  echo "checked";
}
echo ">(Optional)  Show This Computer's Exact Location on Public QCN Maps and Listings <P></td></tr>";

// add radio buttons for type of alignment
echo "<tr><td colspan=2>";
echo "(Optional)  Sensor alignment state: &nbsp&nbsp&nbsp
<select id=\"cboAlign\" name=\"cboAlign\">
  <option value=0 ";
  if ($iRadioAlign==0) echo "selected=\"selected\"";
  echo ">Unaligned</option>
   <option value=1 ";
  if ($iRadioAlign==1) echo "selected=\"selected\"";
  echo ">Magnetic North</option>
   <option value=2 ";
  if ($iRadioAlign==2) echo "selected=\"selected\"";
  echo ">South</option>
   <option value=3 "; 
  if ($iRadioAlign==3) echo "selected=\"selected\"";
  echo  ">East</option>
   <option value=4 ";
  if ($iRadioAlign==4) echo "selected=\"selected\"";
  echo ">West</option>
  <option value=5 ";
  if ($iRadioAlign==5) echo "selected=\"selected\"";
   echo ">Wall</option>
  <option value=6 "; 
  if ($iRadioAlign==6) echo "selected=\"selected\"";
   echo ">True North</option>
</select>
";

echo "</td></tr>";

row2("", "<input type=submit value='Update info'>");

   //print prior geoip lookups
   echo "\n<tr><td $COLSPAN></td></tr><tr><th $COLSPAN align=center>Maxmind/GeoIP Lookups for this Host (based on triggers)</th></tr>\n";
   echo "<tr><th></th><th></th><th>Latitude</th><th>Longitude</th><th>Net (IP) Addr</th><th colspan=3></th></tr>\n";
   for ($i = 0 ; $i < $hllsize ; $i++) {
      if ($hll[$i][0] > 0) { // it's a geoip lookup
         // geoipaddrid,ipaddr,location,latitude,longitude
         echo "<tr><td></td><td></td><td>" . $hll[$i][3] . "</td><td>" . $hll[$i][4] . "</td><td>" . $hll[$i][1] . "</td><td colspan=3></td></tr>\n";
      }
   } 

end_table();
echo "   <input type=\"hidden\" name=\"txthidMAPEXACT\" id=\"txthidMAPEXACT\" value=\"" . $bMapExact . "\">\n";
echo "   <input type=\"hidden\" name=\"txthidHOST\" id=\"txthidHOST\" value=\"" . $host->id . "\">\n";
echo "   <input type=\"hidden\" name=\"txthidIP\" id=\"txthidIP\" value=\"" . $_SERVER['REMOTE_ADDR'] . "\">\n";
echo "   <input type=\"hidden\" name=\"txthidLEVEL\" id=\"txthidLEVEL\" value=\"" . $clvlmax . "\">\n";
echo "</form>\n";

page_tail();

?>
