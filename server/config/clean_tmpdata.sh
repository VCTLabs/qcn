#!/bin/bash
rm -rf /var/www/boinc/sensor/html/stats_*
rm -rf /var/www/boinc/continual/html/stats_*
rm -f /var/www/boinc/sensor/html/user/data/*
rm -f /var/www/boinc/continual/html/user/data/*
rm -f /tmp/tmpFile*
cd /var/www/boinc/sensor
bin/stop
rm -f /var/www/boinc/sensor/log_qcn-web/*
bin/start
cd /var/www/boinc/continual
bin/stop
rm -f /var/www/boinc/continual/log_qcn-web/*
bin/start
