CFLAGS = -g -O0 -Wall
CFLAGS = -O2 -Wall
CC = g++
BOINCDIR = ../../../boinc
MYSQLDIR = /usr/local/mysql
SSLDIR = /usr/local/ssl

LIBS = -L$(MYSQLDIR)/lib -lmysqlclient_r \
  -L$(BOINCDIR)/lib -lboinc \
  -L$(SSLDIR)/lib -lcrypto -lssl \
  -L/usr/local/lib -lcurl

INCLUDES = -I$(BOINCDIR) -I$(BOINCDIR)/lib -I$(BOINCDIR)/tools -I$(BOINCDIR)/db \
           -I$(MYSQLDIR)/include -I$(BOINCDIR)/sched -I/usr/local/include -I../../client/util

SRCS = $(BOINCDIR)/db/db_base.cpp \
  $(BOINCDIR)/lib/sched_msgs.cpp \
  $(BOINCDIR)/sched/sched_limit.cpp \
  $(BOINCDIR)/sched/sched_util.cpp \
  $(BOINCDIR)/sched/sched_config.cpp \
  $(BOINCDIR)/lib/parse.cpp \
  $(BOINCDIR)/db/boinc_db.cpp 

MAIN = qcn_trigmon.cpp qcn_post.cpp

all: clean qcn_trigmon

qcn_trigmon: 
	$(CC) $(CFLAGS) $(INCLUDES) $(MAIN) $(SRCS) $(LIBS) -o qcn_trigmon 
	strip qcn_trigmon
	cp qcn_trigmon /var/www/boinc/sensor/bin/

clean:
	rm -f *.o core *~ qcn_trigmon


