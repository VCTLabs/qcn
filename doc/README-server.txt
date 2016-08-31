The latest QCN/BOINC server-side setup is as follows:

1) Web and data server:  This will have the Apache HTTPD web server instance and appropriate libraries ie PHP, mySQL etc
2) Database server:  This will contain the mySQL database server 


The web server (qcn.usc.edu) should be available to the public for HTTP port 80.  The main website is currently (as of 9/2016) WordPress but that will change when IRIS takes over and puts up their own website.

The WordPress (main) website is under /var/www/qcnwp   (the mySQL database for WordPress is "qcnwp")

The BOINC parts (ie what the client software connects to) of the website are undr /var/www/boinc -- there are further subdrectories "sensor" and "continual" for the respective parts of the project (ie the "continual" polling of a clients sensor; versus the usual "trigger only" polling which is called the "sensor" project).

At this point it may be useful to look at the BOINC project server setup (ie the generic open-source software that runs much of QCN):

https://boinc.berkeley.edu/trac/wiki/ServerIntro


