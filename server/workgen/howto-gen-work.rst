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

So the prefix appears to be 'qcn' for both sensor and continual (since the
template files under /var/www/boinc/continual are identical.  First test of
the arguments:

* wu_prefix = qcn
* num_wu = 100
* appname = sensor

  $ bin/qcn_workgen qcn 100 sensor

If you gat an app ID error, then check database connectivity, permissions,
etc.

