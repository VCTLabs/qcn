#/bin/sh

#
# See: http://boinc.berkeley.edu/trac/wiki/AndroidBuildApp#
#

# Script to compile various BOINC libraries for Android to be used
# by science applications

COMPILEBOINC=
COMPILENTP=
CONFIGURE=yes
MAKECLEAN=yes

export BOINC="../boinc" #BOINC source code

export ANDROIDTC="$HOME/androidarm-tc"
export TCBINARIES="$ANDROIDTC/bin"
export TCINCLUDES="$ANDROIDTC/arm-linux-androideabi"
export TCLIBRARIES="$TCINCLUDES/lib/armv7-a"
export TCSYSROOT="$ANDROIDTC/sysroot"
export STDCPPTC="$TCLIBRARIES/libstdc++.a"

export PATH="$TCBINARIES:$TCINCLUDES/bin:$PATH"
export CXX=arm-linux-androideabi-g++
export LD=arm-linux-androideabi-ld
export CFLAGS="-I$HOME/projects/boinc/api -I$HOME/projects/boinc/lib --sysroot=$TCSYSROOT -DANDROID -DDECLARE_TIMEZONE -Wall -I$TCINCLUDES/include -O3 -fomit-frame-pointer -fPIE"
export CXXFLAGS="-I$HOME/projects/boinc/api -I$HOME/projects/boinc/lib --sysroot=$TCSYSROOT -DANDROID -Wall -I$TCINCLUDES/include -funroll-loops -fexceptions -O3 -fomit-frame-pointer -fPIE"
export LDFLAGS="-L$TCLIBRARIES -lstdc++ -L$TCSYSROOT/usr/lib -L$TCINCLUDES/lib -L$HOME/projects/qcn/client/android -lboinc -lboinc_api -lboinc_zip -llog -fPIE -pie"
export GDB_CFLAGS="--sysroot=$TCSYSROOT -Wall -g -I$TCINCLUDES/include"
export PKG_CONFIG_SYSROOT_DIR=$TCSYSROOT

# Prepare android toolchain and environment
./build_androidtc_arm.sh

if [ -n "$COMPILEBOINC" ]; then
echo "==================building Libraries from $BOINC=========================="
cd $BOINC
if [ -n "$MAKECLEAN" ]; then
make clean
fi
if [ -n "$CONFIGURE" ]; then
./_autosetup
./configure --host=arm-linux --with-boinc-platform="arm-android-linux-gnu" --disable-client --disable-server --disable-manager --disable-shared --enable-static
fi
make
make stage

echo "=============================BOINC done============================="
fi

if [ -n "$COMPILENTP" ]; then
# NTP
echo "=============================NTP start============================="
cd ntp-4.2.6p5
./configure --host=arm-linux --disable-shared --enable-static
make clean 
cd libntp 
make clean && make
cd ../ntpdate
make clean && make
echo "=============================NTP done============================="
fi

cp configure.ac_android_arm configure.ac
cp Makefile.am_android_arm Makefile.am
./_autosetup
./configure --host=arm-linux --with-boinc-platform="arm-android-linux-gnu" --disable-client --disable-server --disable-manager --disable-shared --enable-static
cp version.h.bak version.h
cp configure configure_android_arm
make clean 
make

