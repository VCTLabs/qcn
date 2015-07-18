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
define('CACHE_PATH_MAPTRIG_H', "../cache/maptrigc_h");
define('CACHE_PATH_MAPTRIG_D', "../cache/maptrigc_d");
define('CACHE_PATH_MAPTRIG_W', "../cache/maptrigc_w");
define('CACHE_PATH_MAPTRIG_M', "../cache/maptrigc_m");

define('CACHE_PATH_MAPTRIG_CONTINUAL_H',   "../cache/maptrig_continual_h");
define('CACHE_PATH_MAPTRIG_CONTINUAL_D',   "../cache/maptrig_continual_d");
define('CACHE_PATH_MAPTRIG_CONTINUAL_W',   "../cache/maptrig_continual_w");
define('CACHE_PATH_MAPTRIG_CONTINUAL_M',   "../cache/maptrig_continual_m");

?>
