#!/bin/bash
FILE_BACKUP=/data/QCN/qcn-web-backup-usc.tgz
/bin/rm -f $FILE_BACKUP
cd /var/www
/bin/nice -n19 /bin/tar -czf $FILE_BACKUP \
  --exclude "boinc/sensor/pid_qcn-web/*" \
  --exclude "boinc/sensor/pid_qcn-web" \
  --exclude "boinc/sensor/log_qcn-web/*" \
  --exclude "boinc/sensor/log_qcn-web" \
  --exclude "boinc/sensor/tmp_qcn-web/*" \
  --exclude "boinc/sensor/tmp_qcn-web" \
  --exclude "boinc/mpitest/*" \
  --exclude "boinc/mpitest" \
  --exclude "boinc/sensor/html/cache/*" \
  --exclude "boinc/sensor/html/cache" \
  --exclude "boinc/continual/pid_qcn-web/*" \
  --exclude "boinc/continual/pid_qcn-web" \
  --exclude "boinc/continual/log_qcn-web/*" \
  --exclude "boinc/continual/log_qcn-web" \
  --exclude "boinc/continual/tmp_qcn-web/*" \
  --exclude "boinc/continual/tmp_qcn-web" \
  --exclude "boinc/continual/html/cache/*" \
  --exclude "boinc/continual/html/cache" \
 boinc/ qcn/ qcnwp/ \
 1>/root/backupweb.log 2>/root/backupweb.err
# note for the following line need to have .ssh id between root & carlgt1 setup
# also make sure that '.ssh/known_hosts' is setup to receive connections
#/bin/nice -n19 /usr/bin/scp /tmp/qcn-web-backup.tgz carlgt1@data-private:
#/bin/nice -n19 /bin/mv -f /tmp/qcn-web-backup.tgz /data/QCN/

