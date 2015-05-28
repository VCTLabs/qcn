
/usr/local/mysql/bin/mysqldump -t -u root -p -h db-private sensor qcn_trigger --where='time_trigger<1426864933' > sensor_qcn_trigger_1426864933.sql
/usr/local/mysql/bin/mysqldump -t -u root -p -h db-private continual qcn_trigger --where='time_trigger<1426864933' > continual_qcn_trigger_1426864933.sql

DELETE FROM continual.qcn_trigger WHERE time_trigger < 1426864933;
DELETE FROM sensor.qcn_trigger WHERE time_trigger < 1426864933;



