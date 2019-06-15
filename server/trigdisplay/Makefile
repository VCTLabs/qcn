CFLAGS = -O2 -Wall -DUSE_QCN_TRIGGER_MEMORY_TABLE -DQCN_TRIGGER_MEMORY_TABLE -D_USING_TRIGMEM
#CFLAGS = -g -O0 -Wall
CC = g++
BOINCDIR = ../../../boinc
MYSQLDIR = /usr/local/mysql
SSLDIR = /usr/local/ssl

ifeq ($(shell hostname),qcn-web.stanford.edu)
  INSTALLDIR = /var/www/boinc/sensor/bin
  SERVER_URL = -DQCN_SERVER_URL=\"qcn.stanford.edu\"
else
  INSTALLDIR = /home/boinc/web/boinc/sensor/bin
  SERVER_URL = -DQCN_REMOTE -DQCN_SERVER_URL=\"qcn.emsc-csem.org\"
endif


LIBS = -L$(MYSQLDIR)/lib -lmysqlclient \
  -L$(BOINCDIR)/lib -lboinc \
  -L$(SSLDIR)/lib -lcrypto -lssl \
  -L/usr/local/lib -lcurl

INCLUDES = -I$(BOINCDIR) -I$(SSLDIR)/include -I$(BOINCDIR)/lib -I$(BOINCDIR)/tools -I$(BOINCDIR)/db \
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
MAINDISP =  display.cpp

all: clean trigdisplay

trigdisplay: 
	$(CC) $(CFLAGS) $(SERVER_URL) $(INCLUDES) $(MAINDISP) $(SRCS) $(LIBS) -o trigdisplay
	strip trigdisplay
	cp trigdisplay $(INSTALLDIR)


#trigmonitor: 
#	$(CC) $(CFLAGS) $(SERVER_URL) $(INCLUDES) $(MAINMON) $(SRCS) $(LIBS) -o trigmonitor
#	strip trigmonitor
#	cp trigmonitor $(INSTALLDIR)


clean:
	rm -f *.o core *~ trigmonitor trigdisplay $(INSTALLDIR)/trigmonitor $(INSTALLDIR)/trigdisplay


