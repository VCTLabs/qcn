
/usr/local/mysql/bin/mysqldump -t -u root -p -h db-private sensor qcn_trigger --where='time_trigger<1426864933' > sensor_qcn_trigger_1426864933.sql
/usr/local/mysql/bin/mysqldump -t -u root -p -h db-private continual qcn_trigger --where='time_trigger<1426864933' > continual_qcn_trigger_1426864933.sql

LOCK TABLES continual.qcn_trigger WRITE;
DELETE FROM continual.qcn_trigger WHERE time_trigger < 1426864933;
OPTIMIZE TABLE continual.qcn_trigger;
UNLOCK TABLES;

LOCK TABLES sensor.qcn_trigger WRITE;
DELETE FROM sensor.qcn_trigger WHERE time_trigger < 1426864933;
OPTIMIZE TABLE sensor.qcn_trigger;
UNLOCK TABLES;

