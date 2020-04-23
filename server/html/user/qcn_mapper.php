<?php

require_once("/var/www/qcn/inc/utils.inc");

$lon  = $_REQUEST["lon"];
$lat  = $_REQUEST["lat"];

echo "
<html>
  <head>
    <title=\"QCN Regional Maps\">
  </head>
  <body>
    <div id=\"map_container\" style=\"width: 700px; height: 351px; margin: 0 auto 0 auto;\">
    <h4>QuakeCatcher Network Regional Maps</h4>
    <div id=\"map\" style=\"width: 622px; height: 350px; float: left;\"></div></div>

    <script async defer
      src=\"https://maps.googleapis.com/maps/api/js?key=AIzaSyCi5xpjvTma1yl-cDMtmLqyvt7g3V4dcYk&callback=initMap&sensor=false&v=3\">
    </script>
  </body>
</html>\n

<script>

// Initialize and add the map
function initMap() {

  var image = new google.maps.MarkerImage('mapIcons/marker_red.png',
           // This marker is 20 pixels wide by 32 pixels tall.
           new google.maps.Size(20, 34),
           // The origin for this image is 0,0.
           new google.maps.Point(0,0),
           // The anchor for this image is the base of the flagpole at 0,32.
           new google.maps.Point(9, 34)
        );
  
  var shadow = new google.maps.MarkerImage(
     'http://www.google.com/mapfiles/shadow50.png',
     // The shadow image is larger in the horizontal dimension
     // while the position and offset are the same as for the main image.
     new google.maps.Size(37, 34),
     new google.maps.Point(0,0),
     new google.maps.Point(9, 34)
  );
  
  // Shapes define the clickable region of the icon.
  // The type defines an HTML &lt;area&gt; element 'poly' which
  // traces out a polygon as a series of X,Y points. The final
  // coordinate closes the poly by connecting to the first
  // coordinate.
  
  var shape = {
     coord: [9,0,6,1,4,2,2,4,0,8,0,12,1,14,2,16,5,19,7,23,8,26,9,30,9,34,11,34,11,30,12,26,13,24,14,21,16,18,18,16,20,12,20,8,18,4,16,2,15,1,13,0],
     type: 'poly'
  };

  // The location of Uluru
  var uluru = {lat: $lat, lng: $lon};

  // The marker, positioned at Uluru
  var location = new google.maps.LatLng($lat,$lon);

  var marker = new google.maps.Marker({position: location, map: map});

  var map = new google.maps.Map(document.getElementById('map'), {
      zoom: 15,
      center: location,
      mapTypeId: google.maps.MapTypeId.ROADMAP
  });

  var infowindow = new google.maps.InfoWindow();

  var sensor = 'Sensor';
  var color = 'red';

  var marker = new google.maps.Marker({
      position: location,
      map: map,
      icon: new google.maps.MarkerImage(\"https://labs.google.com/ridefinder/images/mm_20_\" + color + \".png\"),
//        shape: shape
  });

  var i = 1;

  google.maps.event.addListener(marker, 'click', (function(marker, i) {
    return function() {
      infowindow.setContent(\"Hello, world!\");
      infowindow.open(map, marker);
      infowindow.maxWidth(200);
    };
  })(marker, i));
};
  
</script>\n
";

?>
