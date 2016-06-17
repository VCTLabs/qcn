#!/bin/bash
  cd zlib-1.2.5
  ./configure --static
  make clean && make
  cp libz.a ../client/linux_build/

  cd ../bzip2-1.0.6
  make clean && make
  cp libbz2.a ../client/linux_build/

  # make sure there's a /usr/local/bin/libtool for jpeg-9b
  cd ../jpeg-9b
  ./configure --enable-static
  make clean && make
  cp .libs/libjpeg.a ../client/linux_build/

  cd ../curl-7.49.1
  ./configure --disable-crypto-auth --without-ssl --disable-ldap --disable-ldaps --disable-telnet --enable-static --disable-shared
  make clean && make
  cp lib/.libs/libcurl.a ../client/linux_build/

  cd ../freetype-2.6
  ./configure --enable-shared=no --enable-static=yes
  make clean && make
  cp objs/.libs/libfreetype.a ../client/linux_build/

  export CFLAGS=-I$PWD/../freetype-2.6/include
  export CPPFLAGS=-I$PWD/../freetype-2.6/include
  export CXXFLAGS=-I$PWD/../freetype-2.6/include
  cd ../ftgl-2.1.3
  ./configure --enable-shared=no --enable-static=yes
  make clean && make
  cp src/.libs/libftgl.a ../client/linux_build/
  export CFLAGS=
  export CPPFLAGS=
  export CXXFLAGS=

  #  Note - probably need to edit boinc/configure.ac & Makefile.am to add zip/Makefile and zip as a subdir
  #  check qcn/boinc_configure.ac and qcn/boinc_Makefile.am, but basically:
  #  i.e. in Makefile.am should read:
  #    if ENABLE_LIBRARIES
  #       API_SUBDIRS = api lib zip zip/zip zip/unzip
  #    endif
  #  in configure.ac should read:
  #               zip/Makefile
  #               zip/zip/Makefile
  #               zip/unzip/Makefile
  # always build the latest boinc libs
  cd ../../boinc
  ./_autosetup
  ./configure --disable-client --disable-server --enable-libraries
  make clean && make
  cp zip/libboinc_zip.a ../qcn/client/linux_build/
  cp lib/libboinc.a ../qcn/client/linux_build/
  cp api/libboinc_api.a ../qcn/client/linux_build/
  cp api/libboinc_graphics2.a ../qcn/client/linux_build/
  cd ../qcn

