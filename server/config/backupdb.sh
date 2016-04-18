#!/bin/bash
LOCAL_FILE=/data/QCN/qcn-backup.sql
#LOCAL_FILE_GZ=/tmp/qcn-backup.sql.gz
#REMOTE_DIR=/data/QCN/
/bin/rm -f $LOCAL_FILE_GZ
/bin/rm -f $LOCAL_FILE
/bin/nice -n19 /usr/local/mysql/bin/mysqldump -u root -pPWD --single-transaction --databases qcnwp sensor continual sensor_download continual_download qcn_ecommerce todolist > $LOCAL_FILE
/bin/nice -n19 /bin/gzip -f $LOCAL_FILE
# note for the following line need to have .ssh id between root & carlgt1 setup
# also make sure that '.ssh/known_hosts' is setup to receive connections
#/bin/nice -n19 /usr/bin/scp -C $LOCAL_FILE_GZ carlgt1@qcn-upl:
#/bin/nice -n19 /bin/mv -f $LOCAL_FILE_GZ $REMOTE_DIR
