This is code that is included in the boinc/sched build process so that
msg_from_host insert's are intercepted and put into the qcn_trigger table
(built by using ../qcn-data.sql)

Note that triggers never actually end up in msg_from_host but go right to
qcn_trigger (with associated data in the qcn_host_ipaddr and qcn_geo_ipaddr
lookup tables).

Important Note:  In the boinc/sched/Makefile you will need to add -lcurl to
the cgi_LDADD library list (as curl is used for the maxmind/geoip lookup)

Usually just edit the line in boinc/sched/Makefile:

cgi_LDADD = $(LDADD)

to be:

cgi_LDADD = -lcurl $(LDADD)

This should already be in the Makefile under qcn/server/boincmods/sched/Makefile.am

Also note you need to copy the qcn/server/boincmods files to their appropriate
place (boinc/db or boinc/sched) as this has the appropriate lines to include
these mods for the triggers.  This should already be in the qcn/build_server
script.

