<?php
if (file_exists("../inc/common.inc"))
   require_once("../inc/common.inc");
elseif (file_exists("inc/common.inc"))
   require_once("inc/common.inc");
elseif (file_exists("common.inc"))
   require_once("common.inc");

 
$file = $_REQUEST["dat"];
$fthumb = $_REQUEST["fthumb"];
view_data($file,$fthumb);

function view_data($file,$fthumb=null){

if (!$fthumb) {$fthumb=400;}
// Check if this is a continual or sensor network station
if (preg_match("/conti/i","substr($file,0,4)")) {
 $url = UPLOADPATH . "/trigger/continual/";
} else {
 $url = UPLOADPATH . "/trigger/";
}
// Set file name


$file_orig = $url.$file;
//echo $file_orig;
if (!file_exists($file_orig)) {
   echo "Waveform Not Found. Try again later.<BR><BR>" . $file_orig;
   return;
}


// Set base directory:
$dir_base = BASEPATH . "/qcnwp/earthquakes/view";
$dir_sub  = basename($file,".zip");
$dir = $dir_base.'/'.$dir_sub;

// If already viewed (and image already generated), then dont remake the image:
 $jpgfile = "$dir/waveform.jpg";
if (!file_exists($jpgfile)) { // CMC note - test the file, not just the dir
// rotateImage($jpgfile,$fthumb);
//} else {

// If not already made, make image:
if (!file_exists($dir))  {
  mkdir($dir);
}

 $file_new = $dir.'/'.$file;

 copy($file_orig,$file_new);

 if(!file_exists($file_new)) {
    echo "File does not exist: $file...\n";
 } else {
    system(SHELL_CMD . " " . BASEPATH . "/qcnwp/earthquakes/view/plot_data.sh $dir > $dir/temp.txt");
   rotateImage($jpgfile,$fthumb);
 }
    $jpgfile = "$dir/waveform.jpg";



}
   echo "<html><head></head><body>\n";
   echo "<img src=\"http://qcn.stanford.edu/earthquakes/view/".$dir_sub."/waveform.jpg\" width=\"".$fthumb."\">\n";
   echo "</body></head>\n";

// Remove all old data so it doesn't build up over time
clear_old_views($dir_base);
}

// Rotate the image:
function rotateImage($filename,$fthumb) {
// File and rotation
$degrees = 270;

// Content type
//header('Content-type: image/jpeg');


// Make Thumbnail:
// makeThumbnail($filename, $endfile, $thumbwidth, $thumbheight, $quality){

// Load
$source = imagecreatefromjpeg($filename);


// Rotate
$rotate = imagerotate($source, $degrees, 0);

// Output
imagejpeg($rotate,$filename,100);

// Clear up memory:
imagedestroy($rotate);
//header_remove('Content-Type: image/jpeg');

}


function resize_01($filename,$fthumb=null) {
if (!$fthumb) {
 $percent = 1.0;
} else {
 $percent = 1.0;//$fthumb;
}

// Content type
header('Content-Type: image/jpeg');

// Get new sizes
list($width, $height) = getimagesize($filename);
$newwidth = $width * $percent;
$newheight = $height * $percent;

// Load
$thumb = imagecreatetruecolor($newwidth, $newheight);
$source = imagecreatefromjpeg($filename);

// Resize
imagecopyresized($thumb, $source, 0, 0, 0, 0, $newwidth, $newheight, $width, $height);

$degrees = 270;
$rotate = imagerotate($thumb, $degrees, 0);

// Output
$path_parts = pathinfo($filename);
$endfile = $path_parts['dirname']."/".$path_parts['basename']."thumb.jpg";
imagejpeg($rotate,$endfile,0.5);

// Clear up memory:
imagedestroy($rotate);
header_remove('Content-Type: image/jpeg');

}



function clear_old_views($dir_in) {
  $days = "3";                                 // Maximum age of "continual" or "qcn" directories. 
  $seconds = ( $days * 24 * 60 * 60 );         // Max age in seconds 
  $files = scandir($dir_in);                   // Examine directory

  foreach ($files as $num => $fname) {         // for each file / directories
    $fname2 = $dir_in."/".$fname;              // Full path & file name
    if ((preg_match("/conti/i","substr($fname,0,4)"))||(preg_match("/qcnwp/i","substr($fname,0,2)"))) {
      if (is_dir($fname2)) {                   // If it is a directory
        if (file_exists("$fname2")) {
         $dtime = (time()-filemtime("$fname2"));
          if ($dtime  > $seconds) {            // If the directory is older than 3 days

            system("rm -r $fname2");
          }
        }
      }
    }
  }
}

?>

