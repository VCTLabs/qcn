<?php

// Define the lifetime of public php pages.
// define to zero to turn of caching for these pages.
define('TEAM_PAGE_TTL', 3600);
define('USER_PAGE_TTL', 3600);
define('USER_HOST_TTL', 3600);
define('USER_PROFILE_TTL', 3600);
define('TOP_PAGES_TTL', 43200);
define('INDEX_PAGE_TTL', 3600);
define('MAPTRIG_TTL', 3600);

// Define the max allowed cache usage and the min allowed free space
// Both default to 100M.
define('MAX_CACHE_USAGE', 104857600);
define('MIN_FREE_SPACE', 104857600);

// Number of page views between cache size checks
define('CACHE_SIZE_CHECK_FREQ', 1000);

// cached data for maps -- note the day/week/month datasets also!
define('CACHE_PATH_MAPTRIG_H', "../cache/maptrig_h");
define('CACHE_PATH_MAPTRIG_D', "../cache/maptrig_d");
define('CACHE_PATH_MAPTRIG_W', "../cache/maptrig_w");
define('CACHE_PATH_MAPTRIG_M', "../cache/maptrig_m");

define('CACHE_PATH_MAPTRIG_CONTINUAL_H',   "../cache/maptrig_continual_h");
define('CACHE_PATH_MAPTRIG_CONTINUAL_D',   "../cache/maptrig_continual_d");
define('CACHE_PATH_MAPTRIG_CONTINUAL_W',   "../cache/maptrig_continual_w");
define('CACHE_PATH_MAPTRIG_CONTINUAL_M',   "../cache/maptrig_continual_m");

/*
define('CACHE_PATH_MAPTRIG', "../cache/22/maptrig.php_maptrig");
define('CACHE_PATH_MAPTRIG_D', "../cache/22/maptrig.php_maptrig_d");
define('CACHE_PATH_MAPTRIG_W', "../cache/22/maptrig.php_maptrig_w");
define('CACHE_PATH_MAPTRIG_M', "../cache/22/maptrig.php_maptrig_m");

define('CACHE_PATH_MAPTRIG_CONTINUAL',   "../../../continual/html/cache/22/maptrig.php_maptrig");
define('CACHE_PATH_MAPTRIG_CONTINUAL_D', "../../../continual/html/cache/22/maptrig.php_maptrig_d");
define('CACHE_PATH_MAPTRIG_CONTINUAL_W', "../../../continual/html/cache/22/maptrig.php_maptrig_w");
define('CACHE_PATH_MAPTRIG_CONTINUAL_M', "../../../continual/html/cache/22/maptrig.php_maptrig_m");
*/
?>
