unix:BASEDIR = /home/carlgt1/projects
macx:BASEDIR = /Users/carlgt1/projects
win32:BASEDIR = c:/projects

# annoyingly Mac Qt seems to think it's Unix too!
# so use this logical to bypass as can't rely on unix {} conditional
IS_MAC=
mac:IS_MAC=1

BASEDIRBOINC = $$BASEDIR/boinc
BASEDIRQCN = $$BASEDIR/qcn

UTILDIR = $$BASEDIRQCN/client/util
MAINDIR = $$BASEDIRQCN/client/main
SENSORDIR = $$BASEDIRQCN/client/sensor
GRAPHICSDIR = $$BASEDIRQCN/client/graphics
BINDIR = $$BASEDIRQCN/client/bin
BAPIDIR = $$BASEDIRBOINC/api
BLIBDIR = $$BASEDIRBOINC/lib
CURLDIR = $$BASEDIRQCN/curl-7.21.7/include
BZDIR = $$BASEDIRQCN/bzip2-1.0.6

CFLAGS = -Wall -Wno-deprecated
#QMAKE_CFLAGS_DEBUG += $$CFLAGS -D_DEBUG -D_DEBUG_QCNLIVE -g -O0
#QMAKE_CFLAGS_RELEASE += $$CFLAGS -O2
#QMAKE_CXXFLAGS_DEBUG += $$QMAKE_CFLAGS_DEBUG
#QMAKE_CXXFLAGS_RELEASE += $$QMAKE_CFLAGS_RELEASE
HOME = .

# Mac specific settings
macx {
HOME = ..
GCC_VERSION="LLVM GCC 4.2"
QMAKE_CC = llvm-gcc-4.2
QMAKE_CXX = llvm-g++-4.2
QMAKE_MACOSX_DEPLOYMENT_TARGET=10.5
QMAKE_MAC_SDK=/Developer/SDKs/MacOSX10.5.sdk

# my own specific plist not the qmake generated one
QMAKE_INFO_PLIST = "Info.plist.mac"

LIBS += -framework IOKit -framework Carbon \
   -L$$BASEDIRQCN/client/mac_build \
     -lboinc_api -lboinc -lboinc_zip -lboinc_graphics2 \
    -ljpeg-universal -lcurl-universal -lbz2-universal \
     -lz-universal -lfreetype-universal -lftgl-universal

ICON = $$BASEDIRQCN/doc/qcnmac.icns
RC_FILE = $$BASEDIRQCN/doc/qcnmac.icns
QMAKE_INFO_PLIST = Info.plist.mac
}
unix {
 isEmpty(IS_MAC) {
 LIBS += -Wl,-rpath,./init/ --stack=16777216 \
  -L$$BASEDIRQCN/client/linux_build \
   -lcurl -lftgl -lfreetype \
    -lboinc_graphics2 -lboinc_zip -lboinc_api -lboinc \
    -ljpeg
 }
}
win32 {
 LIBS += -L$$BASEDIRQCN/client/win_build, \
c:/projects/qcn/client/win_build,../sensor/motionnodeaccel \
glu32.lib opengl32.lib gdi32.lib user32.lib \
qtmain.lib wsock32.lib hid.lib setupapi.lib winmm.lib \
comctl32.lib boinc_zip.lib curllib.lib jpeglib.lib zlib.lib \
MotionNodeAccelAPI.lib QtOpenGL4.lib QtGui4.lib QtCore4.lib \
ftgl.lib freetype.lib boinc_lib.lib boinc_api.lib
}

DEFINES += _USE_NTPDATE_EXEC_ FTGL_LIBRARY_STATIC QCNLIVE GRAPHICS_PROGRAM APP_GRAPHICS _ZLIB QCN _THREAD_SAFE CURL_STATICLIB _ZLIB $$WINDEF

# setup proper order of include paths
INCLUDEPATH += \
        $$BASEDIRQCN \
        $$BASEDIRQCN/jpeg-6b \
        $$BASEDIRQCN/zlib-1.2.5 \
        $$BZDIR \
        $$CURLDIR \
        $$BASEDIRQCN/ftgl-2.1.3/include \
        $$BASEDIRQCN/freetype-2.4.6/include \
        $$MAINDIR \
        $$SENSORDIR \
        $$UTILDIR \
        $$BASEDIRBOINC/lib \
        $$BASEDIRBOINC/api \
        $$BASEDIRBOINC/zip \
        $$WININCLUDEPATH \
        $$GRAPHICSDIR 


# setup the different sensors required per platform
win32 {
SRC_SENSOR = $$SENSORDIR/csensor_win_laptop_hp.cpp \
           $$SENSORDIR/csensor_win_laptop_thinkpad.cpp \
           $$SENSORDIR/csensor_win_usb_jw.cpp \
           $$SENSORDIR/csensor_win_usb_jw24f14.cpp \
           $$SENSORDIR/csensor_usb_motionnodeaccel.cpp \
           $$SENSORDIR/csensor_win_usb_onavi01.cpp \
           $$SENSORDIR/csensor.cpp
}

macx{
SRC_SENSOR = $$SENSORDIR/csensor_mac_laptop.cpp \
           $$SENSORDIR/csensor_usb_motionnodeaccel.cpp \
           $$SENSORDIR/csensor_mac_usb_onavi01.cpp \
           $$SENSORDIR/csensor_mac_usb_jw.cpp \
           $$SENSORDIR/csensor_mac_usb_jw24f14.cpp \
           $$SENSORDIR/csensor.cpp
}

unix {
 isEmpty(IS_MAC) {
   SRC_SENSOR = \
           $$SENSORDIR/csensor_linux_usb_jw.cpp \
           $$SENSORDIR/csensor_linux_usb_jw24f14.cpp \
           $$SENSORDIR/csensor_usb_motionnodeaccel.cpp \
           $$SENSORDIR/csensor.cpp
 }
}

SRC_MAIN =  $$MAINDIR/qcn_shmem.cpp \
           $$MAINDIR/qcn_thread_sensor.cpp \
           $$MAINDIR/qcn_thread_sensor_util.cpp \
           $$MAINDIR/qcn_thread_sensor_loop.cpp \
           $$MAINDIR/qcn_thread_time.cpp \
           $$MAINDIR/main.cpp

SRC_UTIL = $$UTILDIR/cserialize.cpp \
           $$UTILDIR/sac.cpp \
           $$UTILDIR/qcn_thread.cpp \
           $$UTILDIR/trickleup.cpp \
           $$UTILDIR/trickledown.cpp \
           $$UTILDIR/md5.cpp \
           $$UTILDIR/execproc.cpp \
           $$UTILDIR/qcn_util.cpp \
           $$UTILDIR/qcn_signal.cpp \
           $$UTILDIR/qcn_curl.cpp \
           $$UTILDIR/ttfont.cpp \
           $$UTILDIR/gzstream.cpp

SRC_GRAPHICS = $$GRAPHICSDIR/qcn_graphics.cpp \
      $$GRAPHICSDIR/qcn_2dplot.cpp \
      $$GRAPHICSDIR/qcn_earth.cpp \
      $$GRAPHICSDIR/qcn_cube.cpp \
      $$GRAPHICSDIR/nation_boundary.cpp \
      $$GRAPHICSDIR/coastline.cpp \
      $$GRAPHICSDIR/plate_boundary.cpp

HEADERS       += $$HOME/qcnqt.h $$WININCLUDE \
                $$HOME/glwidget.h \
                $$HOME/myframe.h \
                $$HOME/dlgsettings.h \
                $$HOME/dlgmakequake.h \
                $$HOME/qcnlive_define.h \
                $$HOME/icons.h \
                $$MAINDIR/main.h \
                $$MAINDIR/define.h


SOURCES       = $$HOME/glwidget.cpp \
                $$HOME/qcnqt.cpp \
                $$HOME/myframe.cpp \
                $$HOME/dlgsettings.cpp \
                $$HOME/dlgmakequake.cpp \
                $$SRC_MAIN \
                $$SRC_UTIL \
                $$SRC_SENSOR \
                $$SRC_GRAPHICS 

macx {
CONFIG += x86 x86_64 app_bundle
}
else {
CONFIG += x86 app_bundle
}
QT         += opengl


# install
target.path = $$BINDIR
sources.files = $$SOURCES $$HEADERS $$RESOURCES $$FORMS QCNLive.pro
sources.path = $$BINDIR
INSTALLS += target sources

