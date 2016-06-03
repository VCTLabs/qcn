<?php

// CMC note -- added Google Maps stuff for QCN

require_once("../inc/util.inc");
require_once("common.inc");

$master_url = parse_config(get_config(), "<master_url>");
$is_continual = (parse_config(get_config(), "<db_name>") == "continual");
define ("IS_CONTINUAL", $is_continual ? true : false);
define("PROJECT", "Quake-Catcher Network Sensor Monitoring");
define("URL_BASE", "http://quakecatcher.net/sensor/");
define("MAXMIND_URL", "https://geoip.maxmind.com/b?l=ILFoClxbJcfk");
define("GOOGLE_MAPS_KEY", "AIzaSyDVxlyCoT9_i3eRxVF31dLKLPVTk22ZSS4");
define("IMAGE_PATH", "../user_profile/images/");
define("IMAGE_URL", "user_profile/images/");
define("PROFILE_PATH", "../user_profile/");
define("USE_STOPFORUMSPAM", true);
define("PROFILE_URL", "user_profile/");
define("LANGUAGE_FILE", "languages.txt");
define("STYLESHEET", "qcn.css");
define("COPYRIGHT_HOLDER", "CalTech");
define("SYS_ADMIN_EMAIL", "carlgt1@hotmail.com");
define("UOTD_ADMIN_EMAIL", "carlgt1@hotmail.com"); # who gets user of the day pool running low e-mails?
define("EMAIL_FROM_NAME", "Quake-Catcher Network  (Do Not Reply)");
define("EMAIL_FROM", "noreply@quakecatcher.net");
define("TITLE_COLOR", "");
define("TITLE_FONT", "");
define("BODY_COLOR", "");
define("QUAKE_XML", "../../qcn-quake.xml");
define("MAP_BASE", "img/earth_web.jpg");
define("MAP_TRIGGER", "img/trigger_latest.jpg");
define("MAP_TRIGGER_D", "img/trigger_latest_d.jpg");
define("MAP_TRIGGER_W", "img/trigger_latest_w.jpg");
define("MAP_TRIGGER_M", "img/trigger_latest_m.jpg");
define("MAP_TRIGGER_H", "img/trigger_latest_h.jpg");
define("MIN_MAGNITUDE", "2.0");
define("MIN_MAGNITUDE_D", "3.0");
define("MIN_MAGNITUDE_W", "3.5");
define("MIN_MAGNITUDE_M", "4.0");
define("MIN_MAGNITUDE_H", "3.0");
define("MAX_NUM_TRIGGER", "500");
define("FORUM_QA_MERGED_MODE", true);

// CMC have default global preferences
define("GLOBAL_PREFS_DEFAULT", "<global_preferences>\n"
. "<mod_time>" . time() . "</mod_time>\n"
. "<leave_apps_in_memory>0</leave_apps_in_memory>\n"
. "<suspend_while_on_batteries>0</suspend_while_on_batteries>\n"
. "<idle_time_to_run>0</idle_time_to_run>\n"
. "<suspend_if_no_recent_input>0</suspend_if_no_recent_input>\n"
. "<cpu_scheduling_period_minutes>60</cpu_scheduling_period_minutes>\n"
. "<work_buf_min_days>0</work_buf_min_days>\n"
. "<work_buf_additional_days>0.25</work_buf_additional_days>\n"
. "<max_cpus>1</max_cpus>\n"
. "<cpu_usage_limit>100</cpu_usage_limit>\n"
. "<disk_interval>60</disk_interval>\n"
. "<disk_max_used_gb>100</disk_max_used_gb>\n"
. "<disk_max_used_pct>50</disk_max_used_pct>\n"
. "<disk_min_free_gb>0.001</disk_min_free_gb>\n"
. "<vm_max_used_pct>75</vm_max_used_pct>\n"
. "<ram_max_used_busy_pct>50</ram_max_used_busy_pct>\n"
. "<ram_max_used_idle_pct>90</ram_max_used_idle_pct>\n"
. "<max_bytes_sec_down>0</max_bytes_sec_down>\n"
. "<max_bytes_sec_up>0</max_bytes_sec_up>\n"
. "<suspend_cpu_usage>0</suspend_cpu_usage>\n"
. "</global_preferences>"); 

// Email addresses separated by pipe ( | ) that will receive user reported 
// offensive forum posts.
define("POST_REPORT_EMAILS", "carlgt1@hotmail.com|");

define("MAP_ICON_0", "http://www.google.com/intl/en_us/mapfiles/ms/micons/red-dot.png");
define("MAP_ICON_1", "http://www.google.com/intl/en_us/mapfiles/ms/micons/blue-dot.png");
define("MAP_ICON_2", "http://www.google.com/intl/en_us/mapfiles/ms/micons/green-dot.png");
define("MAP_ICON_3", "http://www.google.com/intl/en_us/mapfiles/ms/micons/purple-dot.png");
define("MAP_ICON_4", "http://www.google.com/intl/en_us/mapfiles/ms/micons/yellow-dot.png");

function project_wordpress_header()
{

  // CMC here - disable enter key
echo "
<script type=\"text/javascript\"> 
function stopRKey(evt) { 
  var evt = (evt) ? evt : ((event) ? event : null); 
  var node = (evt.target) ? evt.target : ((evt.srcElement) ? evt.srcElement : null); 
  if ((evt.keyCode == 13) && (node.type==\"text\"))  {return false;} 
} 
document.onkeypress = stopRKey; 
</script>
";

  // CMC end

}

function project_wordpress_header2()
{

echo "
<div id=\"header\">
                <div id=\"header_inner\">
                        
                <div class=\"logo\"><a rel=\"home\" href=\"" . BASEURL . "/\" class=\"site_logo\"><img src=\"" . BASEURL . "/wp-content/uploads/2011/08/QCN_Logo_origional_small.png\" alt=\"QCN\"  /></a></div><!-- .logo --><div id=\"primary_menu\"><div class=\"jqueryslidemenu\"><ul id=\"menu-main-menu\" class=\"\"><li id=\"menu-item-334\" class=\"menu-item menu-item-type-custom menu-item-object-custom menu-item-home\"><a href=\"" . BASEURL . "\"><span>Home</span></a></li>
<li id=\"menu-item-247\" class=\"menu-item menu-item-type-post_type menu-item-object-page\"><a href=\"" . BASEURL . "/about-qcn\"><span>About QCN</span><small>About QCN
</small></a>
<ul class=\"sub-menu\">
        <li id=\"menu-item-243\" class=\"menu-item menu-item-type-post_type menu-item-object-page\"><a href=\"" . BASEURL . "/about-qcn/desktop-network\">Network</a></li>
        <li id=\"menu-item-369\" class=\"menu-item menu-item-type-post_type menu-item-object-page\"><a href=\"" . BASEURL . "/about-qcn/aftershock-program\">Aftershock Program</a></li>
        <li id=\"menu-item-248\" class=\"menu-item menu-item-type-post_type menu-item-object-page\"><a href=\"" . BASEURL . "/about-qcn/contact-us\">Contact Us</a></li>
        <li id=\"menu-item-1106\" class=\"menu-item menu-item-type-post_type menu-item-object-page\"><a href=\"" . BASEURL . "/about-qcn/videos\">Videos</a></li>
</ul>
</li>
<li id=\"menu-item-242\" class=\"menu-item menu-item-type-post_type menu-item-object-page\"><a href=\"" . BASEURL . "/join-qcn\"><span>Join QCN</span><small>Join QCN
</small></a>
<ul class=\"sub-menu\">
        <li id=\"menu-item-37\" class=\"menu-item menu-item-type-post_type menu-item-object-page\"><a href=\"" . BASEURL . "/join-qcn/request-a-sensor\">Request a Sensor</a></li>
        <li id=\"menu-item-628\" class=\"menu-item menu-item-type-post_type menu-item-object-page\"><a href=\"" . BASEURL . "/join-qcn/download-qcn-monitoring\">Download QCN Software</a></li>
        <li id=\"menu-item-58\" class=\"menu-item menu-item-type-post_type menu-item-object-page\"><a href=\"" . BASEURL . "/join-qcn/manualsinstructions\">Installation Instructions</a></li>
</ul>
</li>
<li id=\"menu-item-255\" class=\"menu-item menu-item-type-post_type menu-item-object-page current_page_parent\"><a href=\"" . BASEURL . "/news\"><span>News</span><small>News
</small></a>
<ul class=\"sub-menu\">
        <li id=\"menu-item-884\" class=\"menu-item menu-item-type-taxonomy menu-item-object-category\"><a href=\"" . BASEURL . "/news/updates\">Updates</a></li>
        <li id=\"menu-item-1034\" class=\"menu-item menu-item-type-taxonomy menu-item-object-category current-post-ancestor current-menu-parent current-post-parent\"><a href=\"" . BASEURL . "/news/events\">Events</a></li>
        <li id=\"menu-item-1149\" class=\"menu-item menu-item-type-post_type menu-item-object-page\"><a href=\"" . BASEURL . "/press-releases\">Press Releases</a></li>
        <li id=\"menu-item-882\" class=\"menu-item menu-item-type-taxonomy menu-item-object-category\"><a href=\"" . BASEURL . "/news/inthenews\">In the News</a></li>
</ul>
</li>
<li id=\"menu-item-262\" class=\"menu-item menu-item-type-post_type menu-item-object-page\"><a href=\"" . BASEURL . "/learning\"><span>Learning</span><small>Learning
</small></a>
<ul class=\"sub-menu\">
        <li id=\"menu-item-1129\" class=\"menu-item menu-item-type-post_type menu-item-object-page\"><a href=\"" . BASEURL . "/join-qcn/request-a-sensor\">Request a Sensor</a></li>
        <li id=\"menu-item-256\" class=\"menu-item menu-item-type-post_type menu-item-object-page\"><a href=\"" . BASEURL . "/learning/qcn-interactve\">QCN Interactve</a></li>
        <li id=\"menu-item-257\" class=\"menu-item menu-item-type-post_type menu-item-object-page\"><a href=\"" . BASEURL . "/learning/lessons-and-activities\">Lessons and Activities</a></li>
        <li id=\"menu-item-1126\" class=\"menu-item menu-item-type-post_type menu-item-object-page\"><a href=\"" . BASEURL . "/join-qcn/download-qcn-monitoring\">Download QCN Software</a></li>
        <li id=\"menu-item-259\" class=\"menu-item menu-item-type-post_type menu-item-object-page\"><a href=\"" . BASEURL . "/learning/learn-about-earthquakes\">Learn About Earthquakes</a></li>
        <li id=\"menu-item-260\" class=\"menu-item menu-item-type-post_type menu-item-object-page\"><a href=\"" . BASEURL . "/learning/glossary\">Glossary</a></li>
        <li id=\"menu-item-261\" class=\"menu-item menu-item-type-post_type menu-item-object-page\"><a href=\"" . BASEURL . "/learning/educational-affiliates\">Educational Affiliates</a></li>
</ul>
</li>
<li id=\"menu-item-268\" class=\"menu-item menu-item-type-post_type menu-item-object-page\"><a href=\"" . BASEURL . "/research\"><span>Research</span><small>Research
</small></a>
<ul class=\"sub-menu\">
        <li id=\"menu-item-1128\" class=\"menu-item menu-item-type-post_type menu-item-object-page\"><a href=\"" . BASEURL . "/join-qcn/request-a-sensor\">Request a Sensor</a></li>
        <li id=\"menu-item-265\" class=\"menu-item menu-item-type-post_type menu-item-object-page\"><a href=\"" . BASEURL . "/research/data-downloads\">Data Downloads</a></li>
        <li id=\"menu-item-266\" class=\"menu-item menu-item-type-post_type menu-item-object-page\"><a href=\"" . BASEURL . "/research/development-opportunities\">Development Opportunities</a></li>
        <li id=\"menu-item-1127\" class=\"menu-item menu-item-type-post_type menu-item-object-page\"><a href=\"" . BASEURL . "/join-qcn/download-qcn-monitoring\">Download QCN Software</a></li>
        <li id=\"menu-item-267\" class=\"menu-item menu-item-type-post_type menu-item-object-page\"><a href=\"" . BASEURL . "/research/research-affiliates\">Research Affiliates</a></li>
</ul>
</li>

<li id=\"menu-item-274\" class=\"menu-item menu-item-type-post_type menu-item-object-page\"><a href=\"" . BASEURL . "/my-qcnboinc\"><span>My QCN/BOINC</span><small>My QCN/BOINC
</small></a>
<ul class=\"sub-menu\">
        <li id=\"menu-item-888\" class=\"menu-item menu-item-type-post_type menu-item-object-page\"><a href=\"" . BASEURL . "/my-qcnboinc/support-forum\">Support Forum</a></li>
        <li id=\"menu-item-269\" class=\"menu-item menu-item-type-post_type menu-item-object-page\"><a href=\"" . BASEURL . "/my-qcnboinc/my-account\">My Account</a></li>
        <li id=\"menu-item-270\" class=\"menu-item menu-item-type-post_type menu-item-object-page\"><a href=\"" . BASEURL . "/my-qcnboinc/my-location\">My Location</a></li>
        <li id=\"menu-item-1125\" class=\"menu-item menu-item-type-post_type menu-item-object-page\"><a href=\"" . BASEURL . "/join-qcn/download-qcn-monitoring\">Download QCN Software</a></li>
        <li id=\"menu-item-272\" class=\"menu-item menu-item-type-post_type menu-item-object-page\"><a href=\"" . BASEURL . "/my-qcnboinc/user-database\">User Database</a></li>
        <li id=\"menu-item-273\" class=\"menu-item menu-item-type-post_type menu-item-object-page\"><a href=\"" . BASEURL . "/my-qcnboinc/statistics\">Statistics</a></li>
</ul>
</li>
</ul></div><div class=\"clearboth\"></div></div><!-- #primary_menu --></div><!-- #header_inner -->
        </div><!-- #header -->
";


echo "

<script type='text/javascript'>
function wp_attempt_focus() {
setTimeout( function() {
try {
d = document.getElementById('user_login');
d.value = '';
d.focus();
} catch(e){}
}, 200 );
}
wp_attempt_focus();
if(typeof wpOnload=='function')wpOnload()
</script>
<script type='text/javascript' src=' " . BASEURL . "/wp-content/themes/persuasion/lib/scripts/prettyphoto/js/jquery.prettyPhoto.js?ver=1.3'></script>
	<script type='text/javascript'>
		// PrintFriendly
		var e = document.createElement('script'); e.type='text/javascript'; e.async = true; 
		e.src = '//cdn.printfriendly.com/printfriendly.js';
		document.getElementsByTagName('head')[0].appendChild(e);
	</script>
				<script type='text/javascript'>/* <![CDATA[ */var ua = jQuery.browser;if( (!ua.msie) || (ua.version.substring(0,1) != '6' && ua.msie) ){Cufon.now();Cufon.replace('h1', { fontFamily: 'colaboratethin' });Cufon.replace('h2', { fontFamily: 'colaboratethin' });Cufon.replace('h3', { fontFamily: 'colaboratethin' });Cufon.replace('#main h4,#intro h4', { fontFamily: 'colaboratelight' });Cufon.replace('.logo a', { fontFamily: 'colaboratethin' });Cufon.replace('#intro .teaser', { fontFamily: 'colaboratelight' });Cufon.replace('#slider_module+#intro h3', { fontFamily: 'colaboratelight' });Cufon.replace('#outro,#outro a', { fontFamily: 'colaboratelight' });Cufon.replace('.toggle a,.toggle_accordion a', { fontFamily: 'colaboratelight' });Cufon.replace('#content .teaser', { fontFamily: 'colaboratelight' });Cufon.replace('#intro .intro_title',{fontFamily: 'colaboratethin',color: '-linear-gradient(#fff,#f9f9f9, #bbb)',textShadow: '1px 2px rgba(0, 0, 0, 0.25), 0px -1px rgba(0, 0, 0, 0.1)',});if ( ua.msie && ua.version.substring(0,1) == '8' ){jQuery('h1,h2,h3,#main h4,#intro h4,.logo a,#intro .teaser,#slider_module+#intro h3,#outro,#outro a,.toggle a,.toggle_accordion a,#content .teaser,#intro .intro_title').css('-ms-filter', '').css('filter', '');}else if( ua.msie && ua.version.substring(0,1) >= '9' ){jQuery('h1,h2,h3,#main h4,#intro h4,.logo a,#intro .teaser,#slider_module+#intro h3,#outro,#outro a,.toggle a,.toggle_accordion a,#content .teaser,#intro .intro_title').css('opacity', '1');}else if(!ua.msie){jQuery('h1,h2,h3,#main h4,#intro h4,.logo a,#intro .teaser,#slider_module+#intro h3,#outro,#outro a,.toggle a,.toggle_accordion a,#content .teaser,#intro .intro_title').css('opacity', '1');}} /* ]]> */</script><script type='text/javascript'>/* <![CDATA[ */jQuery( '.blog_layout1' ).preloader({ imgSelector: '.blog_index_image_load span img', imgAppend: '.blog_index_image_load' });jQuery( '.blog_layout2' ).preloader({ imgSelector: '.blog_index_image_load span img', imgAppend: '.blog_index_image_load' });jQuery( '.blog_layout3' ).preloader({ imgSelector: '.blog_index_image_load span img', imgAppend: '.blog_index_image_load' });jQuery( '.single_post_module' ).preloader({ imgSelector: '.blog_index_image_load span img', imgAppend: '.blog_index_image_load' });jQuery( '.one_column_portfolio' ).preloader({ imgSelector: '.portfolio_img_load span img', imgAppend: '.portfolio_img_load' });jQuery( '.two_column_portfolio' ).preloader({ imgSelector: '.portfolio_img_load span img', imgAppend: '.portfolio_img_load' });jQuery( '.three_column_portfolio' ).preloader({ imgSelector: '.portfolio_img_load span img', imgAppend: '.portfolio_img_load' });jQuery( '.four_column_portfolio' ).preloader({ imgSelector: '.portfolio_img_load span img', imgAppend: '.portfolio_img_load' });jQuery( '.portfolio_gallery.large_post_list' ).preloader({ imgSelector: '.portfolio_img_load span img', imgAppend: '.portfolio_img_load' });jQuery( '.portfolio_gallery.medium_post_list' ).preloader({ imgSelector: '.portfolio_img_load span img', imgAppend: '.portfolio_img_load' });jQuery( '.portfolio_gallery.small_post_list' ).preloader({ imgSelector: '.portfolio_img_load span img', imgAppend: '.portfolio_img_load' });jQuery( '#main_inner' ).preloader({ imgSelector: '.portfolio_full_image span img', imgAppend: '.portfolio_full_image' });jQuery( '#main_inner' ).preloader({ imgSelector: '.blog_sc_image_load span img', imgAppend: '.blog_sc_image_load' });jQuery( '#main_inner' ).preloader({ imgSelector: '.fancy_image_load span img', imgAppend: '.fancy_image_load' });jQuery( '#intro_inner' ).preloader({ imgSelector: '.fancy_image_load span img', imgAppend: '.fancy_image_load' });/* ]]> */</script>

";

echo "
<link rel=\"shortcut icon\" href=\"" . BASEURL . "/wp-content/uploads/2011/08/favicon.png\" />

<script type=\"text/javascript\">
/* <![CDATA[ */
	var imageResize = \"timthumb\",
	    resizeDisabled = \"\",
	    assetsUri = \"" . BASEURL . "/wp-content/themes/persuasion/images/assets\",
            imageNonce = \"de93b49d9b\";
	document.write('<style type=\"text/css\">.noscript{visibility: hidden;}h1,h2,h3,#main h4,#intro h4,.logo a,#intro .teaser,#slider_module+#intro h3,#outro,#outro a,.toggle a,.toggle_accordion a,#content .teaser,#intro .intro_title{opacity: 0;-ms-filter:\"progid:DXImageTransform.Microsoft.Alpha(Opacity=0)\";}</style>');
/* ]]> */
</script>
<meta name='robots' content='noindex,nofollow' />
<link rel='stylesheet' id='my-style-css'  href='" . BASEURL . "/wp-content/plugins/wp-font-resizer-widget/jf.css?ver=3.2.1' type='text/css' media='all' />
<link rel='stylesheet' id='theme-my-login-css'  href='" . BASEURL . "/wp-content/plugins/theme-my-login/theme-my-login.css?ver=6.1.4' type='text/css' media='all' />
<link rel='stylesheet' id='tfg_style-css'  href='" . BASEURL . "/wp-content/plugins/twitter-facebook-google-plusone-share/tfg_style.css?ver=3.2.1' type='text/css' media='all' />
<link rel='stylesheet' id='mysite_prettyphoto-css'  href='" . BASEURL . "/wp-content/themes/persuasion/lib/scripts/prettyphoto/css/prettyPhoto.css?ver=1.3' type='text/css' media='screen' />
<link rel='stylesheet' id='bbpress-style-css'  href='" . BASEURL . "/wp-content/plugins/bbpress/bbp-themes/bbp-twentyten/css/bbpress.css?ver=20110912' type='text/css' media='screen' />
<link rel='stylesheet' id='transposh_widget-css'  href='" . BASEURL . "/wp-content/plugins/transposh-translation-filter-for-wordpress/widgets/flags/tpw_flags.css?ver=0.7.6' type='text/css' media='all' />
<script type='text/javascript' src='" . BASEURL . "/wp-includes/js/l10n.js?ver=20101110'></script>
<script type='text/javascript' src='" . BASEURL . "/wp-includes/js/prototype.js?ver=1.6.1'></script>
<script type='text/javascript' src='" . BASEURL . "/wp-includes/js/scriptaculous/wp-scriptaculous.js?ver=1.8.3'></script>
<script type='text/javascript' src='" . BASEURL . "/wp-includes/js/scriptaculous/effects.js?ver=1.8.3'></script>
<script type='text/javascript'>
/* <![CDATA[ */
var SBCS = {
	sidebarID: \"sidebar\",
	sidebarListEl: \"UL\",
	sidebarListItemEl: \"LI\",
	sidebarListItemPartialClassName: \"widget\",
	sidebarWidgetTitlePartialClassName: \"widgettitle\",
	show: \" [+]\",
	hide: \" [-]\",
	widgetidsdefaultcollapse: \"\"
};
/* ]]> */
</script>
<script type='text/javascript' src='" . BASEURL . "/wp-content/plugins/sidebar-widget-collapser/SidebarCollapserScript.js?ver=1.4'></script>
<script type='text/javascript' src='" . BASEURL . "/wp-content/plugins/wp-font-resizer-widget/jq.js?ver=3.2.1'></script>
<script type='text/javascript' src='" . BASEURL . "/wp-content/plugins/wp-font-resizer-widget/jq1.js?ver=3.2.1'></script>
<script type='text/javascript' src='http://platform.twitter.com/widgets.js?ver=3.2.1'></script>
<script type='text/javascript' src='http://apis.google.com/js/plusone.js?ver=3.2.1'></script>
<script type='text/javascript' src='" . BASEURL . "/wp-includes/js/jquery/jquery.js?ver=1.6.1'></script>
<script type='text/javascript' src='" . BASEURL . "/wp-content/themes/persuasion/lib/scripts/scrollable.min.js?ver=1.3'></script>
<script type='text/javascript' src='" . BASEURL . "/wp-content/themes/persuasion/lib/scripts/cluetip/jquery.cluetip.js?ver=1.3'></script>
<script type='text/javascript' src='" . BASEURL . "/wp-content/themes/persuasion/lib/scripts/custom.js?ver=1.3'></script>
<script type='text/javascript' src='" . BASEURL . "/wp-content/themes/persuasion/lib/scripts/cufon-yui.js?ver=1.3'></script>
<script type='text/javascript' src='" . BASEURL . "/wp-content/themes/persuasion/lib/scripts/fonts/colaboratethin.js?ver=1.3'></script>
<script type='text/javascript' src='" . BASEURL . "/wp-content/themes/persuasion/lib/scripts/fonts/colaboratelight.js?ver=1.3'></script>
<script type='text/javascript' src='" . BASEURL . "/wp-content/plugins/posttabs/postTabs.js?ver=3.2.1'></script>
<link rel='index' title='QCN' href='" . BASEURL . "' />
<meta name=\"generator\" content=\"WordPress 3.2.1\" />
	<style type=\"text/css\">
	    .postTabs_divs{
	padding: 4px;	
}


.postTabs_titles{
	display:none;	
}

ul.postTabs
	{
	margin:0px 0px 1em !important;
	padding: 0.2em 1em 0.2em 20px !important;
	border-bottom: 1px solid #ccc !important;
	font-size: 11px;
	list-style-type: none !important;
	line-height:normal;
	text-align: left;
	display: block !important;
	background: none;
	}

ul.postTabs li
	{	
	display: inline !important;
	font-size: 11px;
	line-height:normal;
	background: none;
	padding: 0px;
	margin: 0px;
	}
  
ul.postTabs li:before{
content: none;	
}  
  	
ul.postTabs li a
	{
	text-decoration: none;
	background: #f3f3f3;
	border: 1px solid #ccc  !important;
	padding: 0.2em 0.4em !important;
	color: #666 !important;
	outline:none;	
	cursor: pointer;
	
	}
	
ul.postTabs li.postTabs_curr a{
	border-bottom: 1px solid #fff  !important;
	background: #fff;
	color: #000000 !important;
	text-decoration: none;
	
	}

ul.postTabs li a:hover
	{
	color: #666 !important;
	background: #fff;
	text-decoration: none;
	
	}

.postTabsNavigation{
	display: block !important;
	overflow:hidden;
}

.postTabs_nav_next{
	float:right;
}

.postTabs_nav_prev{
	float:left;
}
	</style>
	<meta name='robots' content='noindex,nofollow' />
<link rel=\"alternate\" hreflang=\"af\" href=\"/login?loggedout=true&lang=af\"/><link rel=\"alternate\" hreflang=\"sq\" href=\"/login?loggedout=true&lang=sq\"/><link rel=\"alternate\" hreflang=\"ar\" href=\"/login?loggedout=true&lang=ar\"/><link rel=\"alternate\" hreflang=\"hy\" href=\"/login?loggedout=true&lang=hy\"/><link rel=\"alternate\" hreflang=\"az\" href=\"/login?loggedout=true&lang=az\"/><link rel=\"alternate\" hreflang=\"eu\" href=\"/login?loggedout=true&lang=eu\"/><link rel=\"alternate\" hreflang=\"be\" href=\"/login?loggedout=true&lang=be\"/><link rel=\"alternate\" hreflang=\"bn\" href=\"/login?loggedout=true&lang=bn\"/><link rel=\"alternate\" hreflang=\"bg\" href=\"/login?loggedout=true&lang=bg\"/><link rel=\"alternate\" hreflang=\"ca\" href=\"/login?loggedout=true&lang=ca\"/><link rel=\"alternate\" hreflang=\"zh\" href=\"/login?loggedout=true&lang=zh\"/><link rel=\"alternate\" hreflang=\"zh-tw\" href=\"/login?loggedout=true&lang=zh-tw\"/><link rel=\"alternate\" hreflang=\"hr\" href=\"/login?loggedout=true&lang=hr\"/><link rel=\"alternate\" hreflang=\"cs\" href=\"/login?loggedout=true&lang=cs\"/><link rel=\"alternate\" hreflang=\"da\" href=\"/login?loggedout=true&lang=da\"/><link rel=\"alternate\" hreflang=\"nl\" href=\"/login?loggedout=true&lang=nl\"/><link rel=\"alternate\" hreflang=\"eo\" href=\"/login?loggedout=true&lang=eo\"/><link rel=\"alternate\" hreflang=\"et\" href=\"/login?loggedout=true&lang=et\"/><link rel=\"alternate\" hreflang=\"fi\" href=\"/login?loggedout=true&lang=fi\"/><link rel=\"alternate\" hreflang=\"fr\" href=\"/login?loggedout=true&lang=fr\"/><link rel=\"alternate\" hreflang=\"gl\" href=\"/login?loggedout=true&lang=gl\"/><link rel=\"alternate\" hreflang=\"ka\" href=\"/login?loggedout=true&lang=ka\"/><link rel=\"alternate\" hreflang=\"de\" href=\"/login?loggedout=true&lang=de\"/><link rel=\"alternate\" hreflang=\"el\" href=\"/login?loggedout=true&lang=el\"/><link rel=\"alternate\" hreflang=\"gu\" href=\"/login?loggedout=true&lang=gu\"/><link rel=\"alternate\" hreflang=\"ht\" href=\"/login?loggedout=true&lang=ht\"/><link rel=\"alternate\" hreflang=\"he\" href=\"/login?loggedout=true&lang=he\"/><link rel=\"alternate\" hreflang=\"hi\" href=\"/login?loggedout=true&lang=hi\"/><link rel=\"alternate\" hreflang=\"hu\" href=\"/login?loggedout=true&lang=hu\"/><link rel=\"alternate\" hreflang=\"is\" href=\"/login?loggedout=true&lang=is\"/><link rel=\"alternate\" hreflang=\"id\" href=\"/login?loggedout=true&lang=id\"/><link rel=\"alternate\" hreflang=\"ga\" href=\"/login?loggedout=true&lang=ga\"/><link rel=\"alternate\" hreflang=\"it\" href=\"/login?loggedout=true&lang=it\"/><link rel=\"alternate\" hreflang=\"ja\" href=\"/login?loggedout=true&lang=ja\"/><link rel=\"alternate\" hreflang=\"kn\" href=\"/login?loggedout=true&lang=kn\"/><link rel=\"alternate\" hreflang=\"ko\" href=\"/login?loggedout=true&lang=ko\"/><link rel=\"alternate\" hreflang=\"la\" href=\"/login?loggedout=true&lang=la\"/><link rel=\"alternate\" hreflang=\"lv\" href=\"/login?loggedout=true&lang=lv\"/><link rel=\"alternate\" hreflang=\"lt\" href=\"/login?loggedout=true&lang=lt\"/><link rel=\"alternate\" hreflang=\"mk\" href=\"/login?loggedout=true&lang=mk\"/><link rel=\"alternate\" hreflang=\"ms\" href=\"/login?loggedout=true&lang=ms\"/><link rel=\"alternate\" hreflang=\"mt\" href=\"/login?loggedout=true&lang=mt\"/><link rel=\"alternate\" hreflang=\"no\" href=\"/login?loggedout=true&lang=no\"/><link rel=\"alternate\" hreflang=\"fa\" href=\"/login?loggedout=true&lang=fa\"/><link rel=\"alternate\" hreflang=\"pl\" href=\"/login?loggedout=true&lang=pl\"/><link rel=\"alternate\" hreflang=\"pt\" href=\"/login?loggedout=true&lang=pt\"/><link rel=\"alternate\" hreflang=\"ro\" href=\"/login?loggedout=true&lang=ro\"/><link rel=\"alternate\" hreflang=\"ru\" href=\"/login?loggedout=true&lang=ru\"/><link rel=\"alternate\" hreflang=\"sr\" href=\"/login?loggedout=true&lang=sr\"/><link rel=\"alternate\" hreflang=\"sk\" href=\"/login?loggedout=true&lang=sk\"/><link rel=\"alternate\" hreflang=\"sl\" href=\"/login?loggedout=true&lang=sl\"/><link rel=\"alternate\" hreflang=\"es\" href=\"/login?loggedout=true&lang=es\"/><link rel=\"alternate\" hreflang=\"sw\" href=\"/login?loggedout=true&lang=sw\"/><link rel=\"alternate\" hreflang=\"sv\" href=\"/login?loggedout=true&lang=sv\"/><link rel=\"alternate\" hreflang=\"tl\" href=\"/login?loggedout=true&lang=tl\"/><link rel=\"alternate\" hreflang=\"ta\" href=\"/login?loggedout=true&lang=ta\"/><link rel=\"alternate\" hreflang=\"te\" href=\"/login?loggedout=true&lang=te\"/><link rel=\"alternate\" hreflang=\"th\" href=\"/login?loggedout=true&lang=th\"/><link rel=\"alternate\" hreflang=\"tr\" href=\"/login?loggedout=true&lang=tr\"/><link rel=\"alternate\" hreflang=\"uk\" href=\"/login?loggedout=true&lang=uk\"/><link rel=\"alternate\" hreflang=\"ur\" href=\"/login?loggedout=true&lang=ur\"/><link rel=\"alternate\" hreflang=\"vi\" href=\"/login?loggedout=true&lang=vi\"/><link rel=\"alternate\" hreflang=\"cy\" href=\"/login?loggedout=true&lang=cy\"/><link rel=\"alternate\" hreflang=\"yi\" href=\"/login?loggedout=true&lang=yi\"/>
";

}


function project_banner($title) {
    // Put your project title and logo here
    echo "<table width=\"100%\">
        <tr>
        <td><h2>$title</h2></td>
        <td align=right>
       </td></tr></table>
    ";
}

function project_footer($show_return, $show_date) {
   // require_once(BASEPATH . "/boinc/sensor/html/inc/google_translate.inc");
   // google_translate_new();
    echo "<br><hr noshade size=1><center>";
    if ($show_return) {
        echo "<a href=\"".URL_BASE."\">Return to ".PROJECT." main page</a><br>\n";
    }
    echo "<br><br>Copyright &copy; ".date("Y ").COPYRIGHT_HOLDER."</center>\n";
    if ($show_date) {
        echo "<font size=-2 color=bbbbbb>Generated ",time_str(time()),"</font>\n";
    }
}

function show_profile_heading1() {
    return "Your personal background.";
}

function show_profile_question1() {
    return "
        Tell us about yourself.
        You could tell us where you're from, your age, occupation, hobbies,
        or anything else about yourself.
    ";
}

function show_profile_heading2() {
    return "Your opinions about " . PROJECT;
}

function show_profile_question2() {
    return "
        Tell us your thoughts about " . PROJECT . "<ol>
        <li>Why do you run " . PROJECT . "?
        <li>What are your views about the project?
        <li>Any suggestions?
        </ol>
    ";
}

function project_workunit($wu){
    //echo "This is displayed on the workunit page";
}

function project_user_summary($user){
    //echo "Text here will go to the user summary page";
}

function project_user_page_private($user){
    //echo "Whatever is here is added to the private account page";
}

function project_user_credit($user){
    //echo "Whatever is here is added to the private account page";
}

function project_forum_post_rules() {
    // Add <li> entries for additional forum post rules
    return "
    ";
}

function printGoogleMapsHeader()
{ // note the key was registered on http://www.google.com/maps/api for quakecatcher.net
//echo "<script src=\"http://maps.google.com/maps?file=api&v=1&key=ABQIAAAA7FMxjQepz_hQElGJ5tGPvhR_-dF_tBKZEzXEmb5veXIjh7ji-BSpO1s_bzlk2XSTyJa-06G_6sf7rw\""; 
  echo "\n<script type=\"text/javascript\" "
     . "src=\"//maps.googleapis.com/maps/api/js?key=" . GOOGLE_MAPS_KEY 
 . "&sensor=false&v=3\"></script>\n";
} 

function printGoogleMapsScript($prefs, $numMarker = 5, $zoomout = 0)
{
if (!$prefs) return;
 
echo "
<script type=\"text/javascript\">
    //<![CDATA[

   var gIW = null;
   var lastOverlay = new Array($numMarker);
   var micon = new Array($numMarker);
   var map;
   var geocoder;
   var iSelect = 0;

   function getElement(aID)
   {
     return (document.getElementById) ? document.getElementById(aID)
                                      : document.all[aID];
   }

   function clickedAddressLookup(addrline) {

    geocoder.geocode( { address: addrline}, function(results, status) {
      if (status == google.maps.GeocoderStatus.OK) {
            if (!results[0].geometry.location) {
              alert(addrline + \" not found\");
            } else { 
              if (iSelect < 0) { iSelect = 0; }
              getElement(\"lat\" + iSelect).value = results[0].geometry.location.lat();
              getElement(\"lng\" + iSelect).value = results[0].geometry.location.lng();;
              getElement(\"lnm\" + iSelect).value = addrline;
              addMarker(results[0].geometry.location);
              clickedRadio(iSelect, 1);
            }
      } else {
        alert(\"Geocode was not successful for the following reason: \" + status);
      }
    });
   }

   function clickedRadio(num, isselected) {
      if (isselected) {
         iSelect = num;
      }
      moveMap(getElement(\"lat\" + iSelect).value, getElement(\"lng\" + iSelect).value, 15);
   }

   function clickedButton(num, bClear) {
      if (bClear) {
         getElement(\"ipa\" + num).value = \"\";
         //getElement(\"hipa\" + num).value = \"\";
      }
      else {
         getElement(\"ipa\" + num).value = getElement(\"txthidIP\").value;
         //getElement(\"hipa\" + num).value = getElement(\"txthidIP\").value;
      }
   }

   function moveMap(lat, lng, zoomout) {
      if (lat != null && lat != \"\" && lng != null && lng != \"\") {
        newpt = new google.maps.LatLng(lat, lng);
        map.setCenter(newpt);
        map.setZoom(zoomout);
      }
   }

  function infoWindowHandler(myIW, myMap, myMarker) {
    if (gIW) gIW.close();
    gIW = myIW;
    gIW.open(myMap, myMarker);
  }

   function formSetPosition(pos, ival) {
      getElement(\"lat\" + ival).value = pos.lat();
      getElement(\"lng\" + ival).value = pos.lng();
   }

   // little function to add a point
   function addMarker(point)
   {
          //alert(\"LatLng=\" + String(point.lat()) + \", \" + String(point.lng()));
          if (lastOverlay[iSelect]) {
             lastOverlay[iSelect].setVisible(false);
             delete lastOverlay[iSelect];
             lastOverlay[iSelect] = null;
          }
          lastOverlay[iSelect] = new google.maps.Marker({
             map: map,
             position: point, 
             draggable: true,
             title: String(iSelect),
             icon: micon[iSelect],
             visible: true
          });
          // alert(\"micon = \" + micon[iSelect]);
          formSetPosition(point, iSelect);
   }
  
   function showmap() 
   {
       map = new google.maps.Map(
          getElement('map'), 
             { 
                 mapTypeId: google.maps.MapTypeId.ROADMAP
             }
          );
      geocoder = new google.maps.Geocoder();
      for (i = 0 ; i < $numMarker ; i++) {
        lastOverlay[i] = null;
        micon[i] = \"\";
      }
   ";

   echo "moveMap(38, -100, 4);\n";

   for ($i = ($numMarker - 1) ; $i >= 0 ; $i--) {
     echo "   micon[" . $i . "] = \"" . constant("MAP_ICON_" . $i) . "\";\n";
     echo "
      if (getElement(\"lat" . $i . "\").value != \"\" && getElement(\"lng" . $i . "\").value != \"\") {
            iSelect = " . $i . ";
            newpt = new google.maps.LatLng(getElement(\"lat" . $i . "\").value, getElement(\"lng" . $i . "\").value);
            moveMap(newpt.lat(), newpt.lng(), " . ($zoomout ? "$zoomout" : "15") . ");
            addMarker(newpt);
      }
     ";
   }

echo "
      iSelect = 0;
      google.maps.event.addListener(map, 'click', function(evt) {
        if (evt && evt.latLng) {
           addMarker(evt.latLng);
        }
      });
  }

";

echo "
    google.maps.event.addDomListener(window, 'load', showmap);
    //]]>
    </script>
  ";
}

// this overrides the util.inc page_head
function page_head($title, $java_onload=null, $title_plain=null, $prefix="", $google=false, $prefs=null, $maptrig=false, $numMarker=5, $zoomout = 0) 
{
    global $caching, $cache_control_extra, $did_page_head;

    $did_page_head = true;
    $stylesheet = URL_BASE.STYLESHEET;
    $rssname = PROJECT . " RSS 2.0";
    $rsslink = URL_BASE."rss_main.php";

       $metahttp = "<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"/>\n"
         . "<link rel=\"shortcut icon\" href=\"" . BASEURL . "/wp-content/uploads/2011/08/favicon.png\" />\n"
         . "<link rel=\"shortcut icon\" href=\"" . BASEURL . "/wp-content/uploads/2011/08/favicon.png\" type=\"image/vnd.microsoft.icon\">\n"
         . "<link rel=\"icon\" href=\"" . BASEURL . "/wp-content/uploads/2011/08/favicon.png\" type=\"image/vnd.microsoft.icon\">\n";

    header("Content-type: text/html; charset=utf-8");
    if (!$caching) {
        header ("Expires: Mon, 26 Jul 1997 05:00:00 UTC");    // Date in the past
        header ("Last-Modified: " . gmdate("D, d M Y H:i:s") . " UTC"); // always modified
        header ("Cache-Control: $cache_control_extra no-cache, must-revalidate, post-check=0, pre-check=0");  // HTTP/1.1
        header ("Pragma: no-cache");                          // HTTP/1.0
    }

     echo "<!DOCTYPE html PUBLIC \"-//W3C//DTD HTML 4.01 Transitional//EN\" \"http://www.w3.org/TR/html4/loose.dtd\">\n";
       if ($google) {
          echo "<html xmlns=\"http://www.w3.org/1999/xhtml\" xmlns:v=\"urn:schemas-microsoft-com:vml\">
           <head>\n<title>" . PROJECT . " - " . $title . "</title>\n" . $metahttp;
          project_wordpress_header();

         printGoogleMapsHeader();

         if (!$maptrig)  {
            printGoogleMapsScript($prefs, $numMarker, $zoomout);
            echo "</head>\n<body>\n";
         }
       }
       else {
          echo "<html>
            <head>\n<title>" . PROJECT . " - " . $title . "</title>\n" .
            $metahttp;
          project_wordpress_header();
           echo "</head>\n<body>\n";
       }

    if (!$title_plain || is_object($title_plain)) {
        echo "<html><head><title>".strip_tags($title)."</title>\n";
    } else {
        echo "<html><head><title>".strip_tags($title_plain)."</title>\n";
    }
    echo "<link rel=stylesheet type=\"text/css\" href=\"".URL_BASE."main.css\" media=\"all\" />
        <link rel=stylesheet type=\"text/css\" href=\"$stylesheet\">
        <link rel=alternate type=\"application/rss+xml\" title=\"$rssname\" href=\"$rsslink\">
        </head>
    ";

       // IE likes this first bit
/*
       if ($google && strpos($_SERVER['HTTP_USER_AGENT'], "MSIE"))
       {
        echo "v\\:* {
          behavior:url(#default#VML);
        }
        ";
       }
*/

    if ($java_onload){
        echo "<body onload=\"".$java_onload."\">";
    } else {
        echo "<body>";
    }
    //display_cvs_versions();

    project_banner($title, $prefix);
    switch($title) {    //kludge
    case tra("Log in"):
    case tra("Create an account"):
    case tra("Server status page"):
        break;
    default:
        show_login_info($prefix);
    }
}

// disguises the curl using fake headers and a fake user agent.
function disguise_curl($url)
{
  $curl = curl_init();

  // Setup headers - I used the same headers from Firefox version 2.0.0.6
  // below was split up because php.net said the line was too long. :/
  $header[0] = "Accept: text/xml,application/xml,application/xhtml+xml,";
  $header[0] .= "text/html;q=0.9,text/plain;q=0.8,image/png,*/*;q=0.5";
  $header[] = "Cache-Control: max-age=0";
  $header[] = "Connection: keep-alive";
  $header[] = "Keep-Alive: 300";
  $header[] = "Accept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7";
  $header[] = "Accept-Language: en-us,en;q=0.5";
  $header[] = "Pragma: "; // browsers keep this blank.

  curl_setopt($curl, CURLOPT_URL, $url);
  curl_setopt($curl, CURLOPT_USERAGENT, 'Googlebot/2.1 (+http://www.google.com/bot.html)');
  curl_setopt($curl, CURLOPT_HTTPHEADER, $header);
  curl_setopt($curl, CURLOPT_REFERER, 'http://www.google.com');
  curl_setopt($curl, CURLOPT_ENCODING, 'gzip,deflate');
  curl_setopt($curl, CURLOPT_AUTOREFERER, true);
  curl_setopt($curl, CURLOPT_RETURNTRANSFER, 1);
  curl_setopt($curl, CURLOPT_TIMEOUT, 10);

  $html = curl_exec($curl); // execute the curl command
  curl_close($curl); // close the connection

  return $html; // and finally, return $html
}

function show_trigger($hostid, $heading, $owner=false)
{

   if ($hostid<1) return;
   // make a table of triggers for this host
   $tr = null;
   $colorPing = "\"#999999\"";
   $db = BoincDb::get();

   $result=$db->do_query("select count(*) from qcn_showhostlocation where hostid=$hostid");
   $bShowHostLocation = false;
   if ($result && mysql_num_rows($result)>0) {
      $row=mysql_fetch_array($result);
      if ($row[0] == 1) $bShowHostLocation = true;
      mysql_free_result($result);
   }

   // if not $owner just show lat/lng rounded to .01 so we're not showing exact addresses of participants!
   if ($owner || $bShowHostLocation) {
      $sqllatlng = "t.latitude lat, t.longitude lng, ";
   }
   else {
      $sqllatlng = "round(t.latitude,2) lat, round(t.longitude,2) lng, ";
   }

   $sql = "select t.id,t.file,t.time_trigger,t.time_received,"
       . "t.time_sync,t.sync_offset,t.significance, t.magnitude, " . $sqllatlng
       . "t.dt, t.numreset, "
       . "s.description, "
       . "t.sw_version,t.os_type,t.runtime_cpu,t.runtime_clock,t.varietyid,t.levelvalue,t.levelid,"
       . "t.received_file,t.file as file_url,t.result_name,t.alignid,t.qcn_quakeid,q.url, if(t.geoipaddrid=0,'N','Y') is_geoip "
       . "from qcn_trigger t "
       . "             LEFT JOIN qcn_sensor s ON t.qcn_sensorid = s.id "
       . "             LEFT JOIN qcn_quake q ON t.qcn_quakeid = q.id "
       . "WHERE hostid=$hostid order by time_trigger desc limit " . MAX_NUM_TRIGGER;
   $result=$db->do_query($sql);
   $j = 0;

   if ($result && mysql_num_rows($result)>0) {  // we found some records
      $i = 0;
      $j = 0;
      while($row=mysql_fetch_array($result)) {
         if ($row[17]==1) { // it's a ping record
           $strTableFont = "<font color=$colorPing>";
           $strFontEnd = "</font>";
         } 
         else {
           $strTableFont = "";
           $strFontEnd   = "";
         }
         if ($i == 0) {
           $lat = $row[8];
           $lon = $row[9];

   if ($j == 0) {

// Plot sensor location (for first trigger only before printing the table of triggers)
    start_table();
    echo "<tr><th align=\"left\">Sensor Location At Last Trigger:</th></tr>\n";
    echo "<tr><td><table width=\"100%\"><tr><td width=\"40%\">\n";
    echo "<iframe src=\"".BASEURL."/earthquakes/view/qcn_map_sensor.php?lon=" . $lon . "&lat=" . $lat . "\" frameborder=\"0\" scrolling=\"no\" width=\"100%\" height=\"365\" align=\"left\"></iframe>\n";
    echo "</td><td>\n";
    echo "</td></tr></tr></table>\n";
    if ($owner) { // Allow admin & user to edit location
      echo "<tr><td><a href=edit_host_info_form.php?hostid=".$hostid.">Click Here To Verify or Select Locations For This Computer</a></td></tr>\n";
    }
    echo "</table>\n";

   echo "<H2>$heading</H2>\n";
   echo "<H3>Last " . MAX_NUM_TRIGGER . " Triggers for this Computer</H3>\n";
   if ($owner) {
   echo "<H5>(NB: <font color=$colorPing>grey text</font> is a 'ping' trigger to see who is running QCN)</H5>\n";}
   start_table();
   echo "<tr><th>Trigger ID</th>
             <th>Trigger Time (UTC)</th>
             <th>Received Time (UTC)</th>
             <th>Sync Time (UTC)</th>
             <th>Sync Offset</th>
             <th>Significance</th>
             <th>Magnitude</th>
             <th>Latitude</th>
             <th>Longitude</th>
             <th>Elevation</th>
             <th>Align</th>
             <th>Delta T</th>
             <th>Num Reset</th>
             <th>Sensor Type</th>
             <th>Version</th>
             <th>GeoIP?</th>";
    if ($owner) {
    echo "
             <th>OS</th>";
    }
    echo "
             <th>WU CPU Time</th>
             <th>WU Run Time</th>
             <th>File Name</th>
             <th>View</th>
             <th>Quake</th>
         </tr>";
      $j = 1;
         } }
     
         $elev = $row[18]; // level/elevation data

/*
mysql> select * from qcn_level;
+----+------------------------------------+
| id | description                        |
+----+------------------------------------+
|  0 | N/A                                | 
|  1 | Floor (+/- above/below surface)    | 
|  2 | Meters (above/below surface)       | 
|  3 | Feet (above/below surface)         | 
|  4 | Elevation - meters above sea level | 
|  5 | Elevation - feet above sea level   | 
+----+------------------------------------+
*/
         switch($row[19]) { // level id
            case 0:  $elev=""; break;
            case 1:  $elev.="Fl"; break;
            case 2:  $elev.="m"; break;
            case 3:  $elev.="f"; break;
            case 4:  $elev.="m(sl)"; break;
            case 5:  $elev.="f(sl)"; break;
         }

         if ($row[17]==1) { // it's a ping record
            $filename = " ping trigger - no file for result " . $row[22];
            $view = "N/A";
         } 
         else {
          if ($row[20] == 100) { // received_file: show file download link
              $filename = "<A HREF=\"" . UPLOADURL . "/trigger/"
               . (IS_CONTINUAL ? "continual/" : "")
                  . $row[21] . "\">$row[1]</A>";
              $view = "<A HREF=\"javascript:void(0)\"onclick=\"window.open('" . BASEURL . "/earthquakes/view/view_data.php?dat=".$row[1]."','linkname','height=500,width=400,scrollbars=no')\">View</a>";
           }
           else {
              $filename = $row[1];
              $view = "N/A";
           }
         }

         switch($row[23]) { // alignid
           case 0:  $alignment = "Un"; break;
           case 1:  $alignment = "MagN"; break;
           case 2:  $alignment = "South"; break;
           case 3:  $alignment = "East"; break;
           case 4:  $alignment = "West"; break;
           case 5:  $alignment = "Wall"; break;
           case 6:  $alignment = "TrueN"; break;
           default: $alignment = "Un"; 
         }

         $quakestuff = "";

        //if ($row[17] != 1 || $owner) {
         if ($i++ % 2) {
             $bgcolor = "\"#F4F4F4\"";
         }
         else {
             $bgcolor = "\"#FFFFFF\"";
         }
         if ($row[24] > 0 ) { // must have matched a quake!
             $bgcolor = "\"#dff0ff\"";  // red background for this row
             $quakestuff = "<A HREF=\"" . $row[25] . "\">" . $row[24] . "</A>";
         }
         echo "    <tr bgcolor=$bgcolor>
                   <td>$strTableFont$row[0]$strFontEnd</td>
                   <td>$strTableFont" . time_str($row[2]) . "$strFontEnd</td>
                   <td>$strTableFont" . time_str($row[3]) . "$strFontEnd</td>
                   <td>$strTableFont". time_str($row[4]) . "$strFontEnd</td>
                   <td>$strTableFont". $row[5] . "$strFontEnd</td>
                   <td>$strTableFont". $row[6] . "$strFontEnd</td>
                   <td>$strTableFont". $row[7] . "$strFontEnd</td>
                   <td>$strTableFont". $row[8] . "$strFontEnd</td>
                   <td>$strTableFont". $row[9] . "$strFontEnd</td>
                   <td>$strTableFont". $elev . "$strFontEnd</td>
                   <td>$strTableFont". $alignment . "$strFontEnd</td>
                   <td>$strTableFont" . ($row[10] ? $row[10] : 0) . "$strFontEnd</td>
                   <td>$strTableFont$row[11]$strFontEnd</td>
                   <td>$strTableFont$row[12]$strFontEnd</td>
                   <td>$strTableFont$row[13]$strFontEnd</td>
                   <td>$strTableFont$row[26]$strFontEnd</td>";
        if ($owner) {
        echo "
                   <td>$strTableFont$row[14]$strFontEnd</td>";
        }
        echo "
                   <td>$strTableFont$row[15]$strFontEnd</td>
                   <td>$strTableFont$row[16]$strFontEnd</td>
                   <td>$strTableFont$filename$strFontEnd</td>
                   <td>$strTableFont$view$strFontEnd</td>
                   <td>$strTableFont$quakestuff$strFontEnd</td>
              </font></tr>";
       //} // dumb logic to suppress ping triggers?
      }
   } else {
    start_table();
    if ($owner) {
       echo "<tr><td><a href=edit_host_info_form.php?hostid=".$hostid.">Click Here To Verify or Select Locations For This Computer</a></td></tr>\n"; 
    }
    echo "<tr><th align=\"left\">No recent triggers found in database.</th></tr></table>\n";
   }

   end_table();



}

function qcn_admin_user_auth($user, $exit_on_nonauth = false)
{
// authenticate admin-level user
$auth = false;
if ($user) {
  if (!isset($user->prefs)) BoincForumPrefs::lookup($user);
  if (substr($user->prefs->special_user, 3, 1) == "1") $auth = true;
}
if ($exit_on_nonauth && !$auth) {
   echo "You are not authorized to use this page.  Please contact a QCN staff member.";
   exit();
}
return $auth;
}


function qcn_admin_user_check($user)
{
// authenticate admin-level user
$auth = false;
if ($user) {
  if (!isset($user->prefs)) BoincForumPrefs::lookup($user);
  if (substr($user->prefs->special_user, 3, 1) == "1") $auth = true;
}
return $auth;
}

?>
