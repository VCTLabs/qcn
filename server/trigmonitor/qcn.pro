#svn co svn+ssh://byildirm@coates.rcac.purdue.edu/home/ba01/u112/byildirm/DG_OCEANWAVE

#on coates you have to module load CGAL before qmake and make

# File generated by kdevelop's qmake manager. 
# ------------------------------------------- 
# Subdir relative project main directory: .
# Target is an application:  ./bin/dg_swan
#  qmake -spec linux-icc
#QMAKE_CFLAGS   +=  -funroll-loops
#QMAKE_CFLAGS   -=  -funroll-loops
QMAKE_CXXFLAGS +=   -o3  -funroll-loops -frounding-math -fPIC -DONLINE
#QMAKE_CFLAGS   -=  g


#when profiling compile and link with -g -pg option, first run program
#you get gmon.out,  use gprof -b /bin/dg_swan gmon.out > res.txt
#go to kprof to open this file

CONFIG  += release
#QMAKE_CFLAGS  +=  --ggdb

BOINCDIR = /home/boinc/projects/boinc
MYSQLDIR = /usr/local/mysql
SSLDIR = /usr/local/ssl
INSTALLDIR = /var/www/boinc/sensor/bin
QCNDIR=/home/boinc/projects/qcn


HEADERS += Crust2.h \
           QCNBounds.h \
           QCNEvent.h \
           QCNTrigger.h \
           QCNSqlQuery.h \
           QCN.h 


SOURCES +=  $${BOINCDIR}/db/db_base.cpp \
            $${BOINCDIR}/sched/sched_msgs.cpp \
            $${BOINCDIR}/sched/sched_util.cpp \
            $${BOINCDIR}/sched/sched_limit.cpp \
            $${BOINCDIR}/sched/sched_config.cpp \
            $${BOINCDIR}/lib/parse.cpp \
            $${BOINCDIR}/db/boinc_db.cpp


	   
SOURCES += Crust2.cpp \
           QCNBounds.cpp \
           QCNEvent.cpp \
           QCNTrigger.cpp \
           QCNSqlQuery.cpp \
           QCN.cpp \
           QCNMain.cpp 
TEMPLATE = app



INCLUDEPATH += $${BOINCDIR} 
INCLUDEPATH += $${BOINCDIR}/lib 
INCLUDEPATH += $${BOINCDIR}/tools
INCLUDEPATH += $${BOINCDIR}/db
INCLUDEPATH += $${BOINCDIR}/sched
INCLUDEPATH += $${MYSQLDIR}/include 
INCLUDEPATH += $${QCNDIR}/server/trigger 
INCLUDEPATH += /usr/local/include
INCLUDEPATH += ../../../client/util
 

 
 
LIBS += -L$${MYSQLDIR}/lib -lmysqlclient_r 
LIBS += -L$${BOINCDIR}/lib -lboinc         
LIBS += -L$${SSLDIR}/lib -lcrypto -lssl    
LIBS += -L/usr/local/lib -lcurl


TARGET = $${INSTALLDIR}/QCNTrigMonitor


