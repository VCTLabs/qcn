How to use add new work units for sensor and continual apps
===========================================================

The generic process starts with one of the work-gen tools; some examples are
included with boinc-server-maker and a couple of API interfaces.  In this case
there is an existing app-specific tool, qcn_workgen (see the README file for
minimal build/usage hints).

There are a few steps to get the system to start queueing and sending new
work units; generating them from input files in the templates directory
with qcn_workgen is only one.

The general process is:

* create the workgen input and template (aka wu and result) files in .xml
* run a workgen tool (eg, qcn_workgen) to create some number of work units
  and put them in the database
* inspect the database

  - mysql> select id,create_time,appid,name,fileset_id from workunit;

  mysql> SELECT id,create_time,appid,name,fileset_id FROM workunit WHERE name LIKE 'qcnuscxx%';

where "xx" in the prefix is what you inserted.

* inspect the app status, as boinc user

  - $ cd /var/www/boinc/sensor
  - $ bin/status
  - tail log_qcn/transitioner.log

Make sure the transitioner is running and log output looks nominal.  Inspect
any critical errors or other anomalies. You should see new work units being
transitioned successfully and eventually displayed on the server status page
as "Tasks ready to send".  Clients should be taking new tasks as well.

Using the workgen tool
======================

For the QCN sensor and continual apps, we have the qcn_workgen tool
(see $src_dir/server/workgen/work.cpp in the qcn source tree) which
gets installed under APP_NAME/bin in the web install root.  Each
boinc server install can be distro or local defaults; for QCN the
web document root with boinc apps is /var/www/boinc/. For the most
part, all commands installed in /var/www/boinc/APP_NAME/bin are
intended to be run from the boinc directory::

  $ sudo su - boinc
  $ cd /var/www/boinc/APP_NAME
  $ bin/status

The qcn_workgen requires 3 arguments::

  $ bin/qcn_workgen --help
  Usage: bin/qcn_workgen wu_prefix num_wu appname
    where wu_template is the prefix for each workunit (e.g. 'qcna')
    and num_wu is the total number of workunits to create
    and appname is the BOINC app name (e.g. 'sensor')
    (usually run in the application boinc directory i.e. cb to /var/www/boinc/sensor  )
  $ ls templates/
  qcn_input_continual.xml  qcn_output_continual.xml
  qcn_input.xml            qcn_output.xml

So the prefix appears to be 'qcnuscX' for both sensor and continual (since the
template files under /var/www/boinc/continual are identical.  First test of
the arguments:

* wu_prefix = qcnusce
* num_wu = 1000
* appname = qcnsensor

  $ bin/qcn_workgen qcnusce 1000 qcnsensor

If you get an app ID error, then check database connectivity, permissions,
etc.  Also make sure you have the right db and app names.

The qcn_workgen tool reads the database connection parameters from the .xml
config files in the current "app" directory (ie, where you run the tool).
The main config file is config.xml; the config_aux file is not being used.

MySQL db settings and corresponding environment
-----------------------------------------------

Both the app and the workgen tool both use the global app config.xml (and
aux if needed).

Make sure there are no conflicting my.cnf settings in the user environment
you are running the tool in.  If you sudo to the boinc or boincadm user to
do this (which you should) make sure you pass the nominal environment::

  $ sudo su - boinc

The "dash" is important for su to pass the right environment variables, etc.

