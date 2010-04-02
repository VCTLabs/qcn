// update host ipaddr's to just use one lat/lng per host

create temporary table tmp_setip 
   (select id from continual.qcn_host_ipaddr where (geoipaddrid is null or geoipaddrid=0) and ipaddr is not null and ipaddr!=''
        and hostid not in (select hostid from continual.qcn_host_ipaddr where ipaddr is null or ipaddr='')
        and hostid not in (select hostid from continual.qcn_host_ipaddr where (geoipaddrid is null or geoipaddrid=0) group by hostid having count(*)>1)
);

update continual.qcn_host_ipaddr set ipaddr='' where id in (select id from tmp_setip);

// update trigger table with "sole location" hosts

update continual.qcn_trigger t, continual.qcn_host_ipaddr i set t.latitude=i.latitude,t.longitude=i.longitude where t.hostid=i.hostid and i.ipaddr='' and i.geoipaddrid=0;

// query to see which hosts in the trigger table have geoipaddr records

select t.hostid, t.ipaddr, count(*) 
   from continual.qcn_trigger t, continual.qcn_host_ipaddr i
   where t.hostid=i.hostid and t.ipaddr=i.ipaddr and i.geoipaddrid>0 
  group by t.hostid, t.ipaddr;


