CFLAGS = -g -Wall -DQCN_SERVER
CC = g++
BOINCDIR = ../../../boinc
MYSQLDIR = /usr/local/mysql

MYSQL_CFLAGS = -I$(MYSQLDIR)/include  -DUNIV_LINUX
MYSQL_LIBS = -rdynamic -L$(MYSQLDIR)/lib -lmysqlclient_r -lz -lrt -lcrypt -lnsl -lm

SSL_CFLAGS = -I/usr/local/ssl/include
SSL_CXXFLAGS = -I/usr/local/ssl/include
SSL_LIBS = -L/usr/local/ssl/lib -lssl -lcrypto -ldl -lz

ifeq ($(shell hostname),qcn-web.caltech.edu)
  QCNDIR = /var/www/boinc/sensor
  CONTDIR = /var/www/boinc/continual
else
  QCNDIR = /home/boinc/web/boinc/sensor
  CONTDIR = /home/boinc/web/boinc/continual
endif

LIBS = -L$(BOINCDIR)/lib -lboinc -lboinc_crypt $(SSL_LIBS) -L$(BOINCDIR)/sched -lsched $(MYSQL_LIBS) $(SSL_LIBS)

INCLUDES = -I$(BOINCDIR) -I$(BOINCDIR)/lib -I$(BOINCDIR)/tools -I$(BOINCDIR)/db \
           -I$(MYSQLDIR)/include -I$(BOINCDIR)/sched

#$(BOINCDIR)/sched/sched_msgs.cpp \

SRCS = $(BOINCDIR)/db/db_base.cpp \
  $(BOINCDIR)/db/boinc_db.cpp \
  $(BOINCDIR)/lib/md5_file.cpp \
  $(BOINCDIR)/sched/sched_config.cpp \
  $(BOINCDIR)/sched/sched_util.cpp \
  $(BOINCDIR)/tools/backend_lib.cpp \
  $(BOINCDIR)/tools/process_result_template.cpp

MAIN = work.cpp

all: qcn_workgen

qcn_workgen: 
	$(CC) $(CFLAGS) $(INCLUDES) $(SRCS) $(LIBS) $(MAIN) -o qcn_workgen
	cp -rf templates/ $(QCNDIR)/
	cp qcn_workgen $(QCNDIR)/bin/
	cp -rf templates/ $(CONTDIR)/
	mv qcn_workgen $(CONTDIR)/bin/

clean:
	rm -f *.o core *~ qcn_workgen


