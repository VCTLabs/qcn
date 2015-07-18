select yearweek(time_trigger) yw, count(*) ctr from sensor.qcn_trigger group by yw;

