#CFLAGS = -O2 -Wall
CFLAGS = -g -O0 -Wall -DUSE_QCN_TRIGGER_MEMORY_TABLE -DQCN_TRIGGER_MEMORY_TABLE -D_USING_TRIGMEM
CC = g++
BOINCDIR = ../../../boinc
MYSQLDIR = /usr/local/mysql
SSLDIR = /usr/local/ssl
INSTALLDIR = /var/www/boinc/sensor/bin

LIBS = -L$(MYSQLDIR)/lib -lmysqlclient \
  -L$(BOINCDIR)/lib -lboinc \
  -L$(SSLDIR)/lib -lcrypto -lssl \
  -L/usr/local/lib -lcurl

INCLUDES = -I$(BOINCDIR) -I$(BOINCDIR)/lib -I$(BOINCDIR)/tools -I$(BOINCDIR)/db \
           -I$(MYSQLDIR)/include -I$(BOINCDIR)/sched -I/usr/local/include -I../../client/util

SRCS = $(BOINCDIR)/db/db_base.cpp \
  $(BOINCDIR)/lib/sched_msgs.cpp \
  $(BOINCDIR)/sched/sched_util.cpp \
  $(BOINCDIR)/sched/sched_util_basic.cpp \
  $(BOINCDIR)/sched/sched_limit.cpp \
  $(BOINCDIR)/sched/sched_config.cpp \
  $(BOINCDIR)/lib/parse.cpp \
  $(BOINCDIR)/db/boinc_db.cpp 

MAINMON  = trigmon.cpp
MAINLOC  = qcn_locate.cpp
MAINDISP =  display.cpp

all: clean trigmonitor trigdisplay
#all: clean trigdisplay
#all: clean trigmonitor

trigdisplay: 
	$(CC) $(CFLAGS) $(INCLUDES) $(MAINDISP) $(SRCS) $(LIBS) -o jfl_trigdisplay
	cp jfl_trigdisplay $(INSTALLDIR)


trigmonitor: 
	$(CC) $(CFLAGS) $(INCLUDES) $(MAINMON) $(SRCS) $(LIBS) -o jfl_trigmonitor
	cp jfl_trigmonitor $(INSTALLDIR)

qcn_locate: 
	$(CC) $(CFLAGS) $(INCLUDES) $(MAINLOC) $(SRCS) $(LIBS) -o qcn_locate
	cp qcn_locate $(INSTALLDIR)


clean:
	rm -f *.o core *~ jfl_trigmonitor jfl_trigdisplay $(INSTALLDIR)/jfl_trigmonitor $(INSTALLDIR)/jfl_trigdisplay


