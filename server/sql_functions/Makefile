## don't forget:
## USE sensor;
## DROP FUNCTION quake_hit_test;
## DROP FUNCTION lat_lon_distance_m;
##
## CREATE FUNCTION lat_lon_distance_m RETURNS REAL SONAME "udf_qcn.so";
## CREATE FUNCTION quake_hit_test RETURNS INTEGER SONAME "udf_qcn.so";
##

CC    = gcc
CXX   = g++
INCLUDEDIR  = -I/usr/local/mysql/include
BINDIR = /usr/local/mysql/lib/plugin/
ISMAC = $(shell uname -s)

ifeq ($(ISMAC), Darwin)
  EXTRA = -dynamiclib
else
  EXTRA = -rdynamic -shared
endif

all:
	rm -f udf_qcn.so
	$(CC) $(INCLUDEDIR) $(EXTRA) -fPIC -o udf_qcn.so udf_qcn.c
	sudo mv udf_qcn.so $(BINDIR)

clean:
	rm -f *.so a.out


