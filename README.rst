Quake Catcher Network (QCN) server and client software
======================================================

This is the working git repository for the Quake-Catcher Network project,

Home Page: http://quakecatcher.net

All files are (c) 2007-2016 Incorporated Research Institutions for Seismology
(IRIS).  Project originally developed at Stanford University (2007-2014), then
hosted at CalTech (mainly on autopilot), now at USC via the Southern California
Earthquake Center (SCEC) with support by IRIS.

Original author unless otherwise noted is Carl Christensen, carlgt1@hotmail.com
Maintenance by Stephen L Arnold <nerdboy@gentoo.org>

This code is released under the Lesser GNU Public License (LGPL) version 3
or higher, with the hopes that it will be useful for academic use and
experimentation in seismology, education, community development, etc.

::

  This program is free software: you can redistribute it and/or modify
  it under the terms of the Lesser GNU General Public License (LGPL) as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  Lesser GNU General Public License for more details.

  You should have received a copy of the Lesser GNU General Public License
  along with this program (e.g. COPYING.LESSER).  If not, see http://www.gnu.org/licenses/lgpl.html


To build the  software, you will need various dependencies, some of which are
included e.g. libcurl etc, but mainly you will need to get the BOINC libraries
cloned parallel to the qcn directory (see below). Basically the QCN client
code is in the subdir "client" and the server code in "server" (duh)

QCN client software
===================

In client there is a subdirectory "qcnlive" for the Qt-based (you'll need to
install Qt) QCNLive program - and makefiles for Linux and visual editor files
for XCode (Mac) and Visual Studio (Windows) precompiled libraries and the
visual studio/xcode files are also in win_build & mac_build respectively.

The Phidgets libraries (v21) are available upstream as deb packages (x86,
armhf, armel) and generic source.

Eg, for Linux follow the Linux guide: https://www.phidgets.com/docs/OS_-_Linux
and install libphidget21-dev for the official USB sensors.


For this branch (Stephen Arnold, July 2017)
-------------------------------------------

Note for the the build steps below, only client/main and client/graphics are
built.  The other (client) targets in Makefile.am* may build on the other two
platforms but are not currently buildable on Linux ARM.

Useful build deps (officially) for OpenEmbedded but good for ARM Linux::

  $ sudo apt-get install bash-completion sed wget cvs subversion git-core \
    coreutils unzip texi2html texinfo docbook-utils gawk diffstat \
    help2man make gcc build-essential g++ desktop-file-utils \
    chrpath libxml2-utils xmlto apache2-utils docbook

Actual build deps for QCN on debian/ubuntu armhf::

  $ apt-get install \
    build-essential \
    m4 \
    autoconf \
    zlib1g-dev \
    libbz2-dev \
    libjpeg-dev \
    libcurl4-openssl-dev \
    libssl-dev \
    libfreetype6-dev \
    libxcb1-dev \
    libxcb-util-dev \
    libgss-dev \
    libgssglue-dev \
    freeglut3-dev \
    libnotify-dev \
    libxcb-screensaver0-dev \
    libxss-dev \
    libxmu-dev \
    libxi-dev \
    libftgl-dev \
    libwxgtk3.0-dev \
    libwxgtk-webview3.0-dev

Note you may also need to install libstdc++6-4.7-dev; the above will pull in
many other dev dependencies as well.

To build native from source on Debian stretch or Ubuntu Xenial (armhf) follow
these steps; you may need to "apt-cache search" to find correct package names
for the above build deps.

Starting in ~/src/::

  $ cd ~/src
  $ git clone https://github.com/VCTLabs/qcn
  $ git clone git://boinc.berkeley.edu/boinc.git
  $ cd boinc
  $ git checkout origin/client_release/7/7.4 -b client-7.4
  $ make distclean
  $ rm -rf libltdl compile depcomp missing
  $ ./_autosetup
  $ ./configure --disable-server --disable-client --enable-shared --enable-static --with-pic
  $ make
  $ sudo make install
  $ make -C zip/
  $ sudo make -C zip/ install

Next build the qcn app::

  $ cd ../qcn
  $ git checkout boinc-7.4.53
  $ make distclean
  $ ./_autosetup

Note: run make distclean and _autosetup again if you get errors and either
configure or _autosetup does not complete.  Now configure the client::

  $ ./configure --disable-server --enable-shared --enable-static  --with-pic
  $ make

Look in client/test/projects/qcn.edu_qcn/ and client/bin/ for the armv7
client binaries.

QCN server software
===================

Although there are several client branches, there is only one branch with the
name "server" in it (setiathome_server) so we'll start with master branch::

  $ cd ~/src/boinc
  $ cp -rf ../server/boincmods/* .
  $ make distclean
  $ ./_autosetup
  $ ./configure --enable-server --disable-client --enable-shared --enable-static --with-pic
  $ make
  $ make -C sched/
  $ make
  $ sudo make install


Code cleanup tasks
==================

* make repo manifest for primary build repos
* make branches	to match workflow
* move old branches to legacy namespace
* remove stale dependencies from source tree

  - fix autotools to use pkg-config instead
  - clean up/refactor configure.ac and associated Makefile.ams

* check stale boincmods in qcn server/ against master

  - merge into local boinc fork
  - make patch(es) and deb package updates
    (need one or more repeaetable build branches)
