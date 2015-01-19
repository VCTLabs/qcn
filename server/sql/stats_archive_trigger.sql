/* for qcn_recalcresult table in do_stats ie to get archive numbers */

SELECT r.id resultid,       exp(-(abs(unix_timestamp()-max(t.time_received))*0.69314718/604800.0)) weight,         (50.0*IF(MAX(t.runtime_clock)>100000.0,100000.0,MAX(t.runtime_clock)))/86400.0 total_credit,           max(t.time_received)
         FROM sensor.result r
           LEFT JOIN sensor.qcn_finalstats f ON r.id=f.resultid
           LEFT JOIN sensor.qcn_trigger q    ON r.name=q.result_name
           JOIN sensor_archive.qcn_trigger t ON r.name=t.result_name
         WHERE f.resultid IS NULL AND q.result_name IS NULL AND t.runtime_clock>0
                   GROUP BY r.id
INTO OUTFILE 'sensor_archive_stats_dump.sql';

SELECT r.id resultid,       exp(-(abs(unix_timestamp()-max(t.time_received))*0.69314718/604800.0)) weight,         (50.0*IF(MAX(t.runtime_clock)>100000.0,100000.0,MAX(t.runtime_clock)))/86400.0 total_credit,           max(t.time_received)
         FROM continual.result r
           LEFT JOIN continual.qcn_finalstats f ON r.id=f.resultid
           LEFT JOIN continual.qcn_trigger q    ON r.name=q.result_name
           JOIN continual_archive.qcn_trigger t ON r.name=t.result_name
         WHERE f.resultid IS NULL AND q.result_name IS NULL AND t.runtime_clock>0
                   GROUP BY r.id
INTO OUTFILE 'continualr_archive_stats_dump.sql';


