#############################################################################
# Makefile for building: qcnlive.app/Contents/MacOS/qcnlive
# Generated by qmake (3.0) (Qt 5.0.0) on: Sat Jan 5 17:26:37 2013
# Project:  qcnlive.pro
# Template: app
# Command: /Developer/Qt5.0.0/5.0.0/clang_64/bin/qmake -spec macx-xcode -o qcnlive.xcodeproj/project.pbxproj qcnlive.pro
#############################################################################

MAKEFILE      = project.pbxproj

MOC       = /Developer/Qt5.0.0/5.0.0/clang_64/bin/moc
UIC       = /Developer/Qt5.0.0/5.0.0/clang_64/bin/uic
LEX       = flex
LEXFLAGS  = 
YACC      = yacc
YACCFLAGS = -d
DEFINES       = -D_USE_NTPDATE_EXEC_ -DFTGL_LIBRARY_STATIC -DQCNLIVE -DGRAPHICS_PROGRAM -DAPP_GRAPHICS -D_ZLIB -DQCN -D_THREAD_SAFE -DCURL_STATICLIB -D_ZLIB -DQT_NO_DEBUG -DQT_OPENGL_LIB -DQT_WIDGETS_LIB -DQT_GUI_LIB -DQT_CORE_LIB
INCPATH       = -I/Developer/Qt5.0.0/5.0.0/clang_64/mkspecs/macx-xcode -I. -I../../../qcn -I/Users/carlgt1/projects/qcn/jpeg-6b -I../../zlib-1.2.5 -I../../bzip2-1.0.6 -I../../curl-7.25.0/include -I../../ftgl-2.1.3/include -I../../freetype-2.4.6/include -I../main -I../sensor -I../util -I../../../boinc/lib -I../../../boinc/api -I../../../boinc/zip -I../graphics -I/Developer/Qt5.0.0/5.0.0/clang_64/include -I/Developer/Qt5.0.0/5.0.0/clang_64/include/QtOpenGL -I/Developer/Qt5.0.0/5.0.0/clang_64/lib/QtOpenGL.framework/Versions/5/Headers -I/Developer/Qt5.0.0/5.0.0/clang_64/include/QtWidgets -I/Developer/Qt5.0.0/5.0.0/clang_64/lib/QtWidgets.framework/Versions/5/Headers -I/Developer/Qt5.0.0/5.0.0/clang_64/include/QtGui -I/Developer/Qt5.0.0/5.0.0/clang_64/lib/QtGui.framework/Versions/5/Headers -I/Developer/Qt5.0.0/5.0.0/clang_64/include/QtCore -I/Developer/Qt5.0.0/5.0.0/clang_64/lib/QtCore.framework/Versions/5/Headers -I. -I/System/Library/Frameworks/OpenGL.framework/Versions/A/Headers -I/System/Library/Frameworks/AGL.framework/Headers -I/usr/local/include -I/System/Library/Frameworks/CarbonCore.framework/Headers
DEL_FILE  = rm -f
MOVE      = mv -f

IMAGES = 
PARSERS =
preprocess: $(PARSERS) compilers
clean preprocess_clean: parser_clean compiler_clean

parser_clean:
mocclean: compiler_moc_header_clean compiler_moc_source_clean

mocables: compiler_moc_header_make_all compiler_moc_source_make_all

check: first

compilers: ./moc_qcnqt.cpp ./moc_glwidget.cpp ./moc_myframe.cpp\
	 ./moc_dlgsettings.cpp ./moc_dlgmakequake.cpp
compiler_objective_c_make_all:
compiler_objective_c_clean:
compiler_rcc_make_all:
compiler_rcc_clean:
compiler_moc_header_make_all: moc_qcnqt.cpp moc_glwidget.cpp moc_myframe.cpp moc_dlgsettings.cpp moc_dlgmakequake.cpp
compiler_moc_header_clean:
	-$(DEL_FILE) moc_qcnqt.cpp moc_glwidget.cpp moc_myframe.cpp moc_dlgsettings.cpp moc_dlgmakequake.cpp
moc_qcnqt.cpp: /Developer/Qt5.0.0/5.0.0/clang_64/include/QtWidgets/QApplication \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtGui/QtGui \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtOpenGL/QtOpenGL \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtWidgets/QMainWindow \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtWidgets/QSplashScreen \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtCore/QTimer \
		../main/main.h \
		../main/define.h \
		../../../qcn/config.h.win \
		../../../qcn/config.h \
		../../../boinc/api/boinc_gl.h \
		../util/qcn_util.h \
		../main/qcn_shmem.h \
		../util/sac.h \
		../util/qcn_thread.h \
		../main/qcn_thread_time.h \
		../main/qcn_thread_sensor.h \
		../util/qcn_signal.h \
		../util/execproc.h \
		../../../boinc/lib/boinc_win.h \
		../sensor/csensor.h \
		../../../boinc/api/boinc_api.h \
		../../../boinc/lib/diagnostics.h \
		../../../boinc/lib/str_util.h \
		../../../boinc/lib/filesys.h \
		../../../boinc/lib/parse.h \
		../../../boinc/lib/mfile.h \
		../../../boinc/api/graphics2.h \
		../../../boinc/lib/util.h \
		../util/qcn_gzip.h \
		../util/trickleup.h \
		../util/trickledown.h \
		../graphics/qcn_graphics.h \
		../util/qcn_curl.h \
		myframe.h \
		qcnqt.h \
		qcnlive_define.h \
		glwidget.h \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtOpenGL/QGLWidget \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtWidgets/QSlider \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtWidgets/QScrollArea \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtWidgets/QLabel \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtWidgets/QDockWidget \
		qcnqt.h
	/Developer/Qt5.0.0/5.0.0/clang_64/bin/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__=4 qcnqt.h -o moc_qcnqt.cpp

moc_glwidget.cpp: qcnqt.h \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtWidgets/QApplication \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtGui/QtGui \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtOpenGL/QtOpenGL \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtWidgets/QMainWindow \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtWidgets/QSplashScreen \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtCore/QTimer \
		../main/main.h \
		../main/define.h \
		../../../qcn/config.h.win \
		../../../qcn/config.h \
		../../../boinc/api/boinc_gl.h \
		../util/qcn_util.h \
		../main/qcn_shmem.h \
		../util/sac.h \
		../util/qcn_thread.h \
		../main/qcn_thread_time.h \
		../main/qcn_thread_sensor.h \
		../util/qcn_signal.h \
		../util/execproc.h \
		../../../boinc/lib/boinc_win.h \
		../sensor/csensor.h \
		../../../boinc/api/boinc_api.h \
		../../../boinc/lib/diagnostics.h \
		../../../boinc/lib/str_util.h \
		../../../boinc/lib/filesys.h \
		../../../boinc/lib/parse.h \
		../../../boinc/lib/mfile.h \
		../../../boinc/api/graphics2.h \
		../../../boinc/lib/util.h \
		../util/qcn_gzip.h \
		../util/trickleup.h \
		../util/trickledown.h \
		../graphics/qcn_graphics.h \
		../util/qcn_curl.h \
		myframe.h \
		glwidget.h \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtOpenGL/QGLWidget \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtWidgets/QSlider \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtWidgets/QScrollArea \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtWidgets/QLabel \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtWidgets/QDockWidget \
		qcnlive_define.h \
		glwidget.h
	/Developer/Qt5.0.0/5.0.0/clang_64/bin/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__=4 glwidget.h -o moc_glwidget.cpp

moc_myframe.cpp: qcnqt.h \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtWidgets/QApplication \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtGui/QtGui \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtOpenGL/QtOpenGL \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtWidgets/QMainWindow \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtWidgets/QSplashScreen \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtCore/QTimer \
		../main/main.h \
		../main/define.h \
		../../../qcn/config.h.win \
		../../../qcn/config.h \
		../../../boinc/api/boinc_gl.h \
		../util/qcn_util.h \
		../main/qcn_shmem.h \
		../util/sac.h \
		../util/qcn_thread.h \
		../main/qcn_thread_time.h \
		../main/qcn_thread_sensor.h \
		../util/qcn_signal.h \
		../util/execproc.h \
		../../../boinc/lib/boinc_win.h \
		../sensor/csensor.h \
		../../../boinc/api/boinc_api.h \
		../../../boinc/lib/diagnostics.h \
		../../../boinc/lib/str_util.h \
		../../../boinc/lib/filesys.h \
		../../../boinc/lib/parse.h \
		../../../boinc/lib/mfile.h \
		../../../boinc/api/graphics2.h \
		../../../boinc/lib/util.h \
		../util/qcn_gzip.h \
		../util/trickleup.h \
		../util/trickledown.h \
		../graphics/qcn_graphics.h \
		../util/qcn_curl.h \
		myframe.h \
		glwidget.h \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtOpenGL/QGLWidget \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtWidgets/QSlider \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtWidgets/QScrollArea \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtWidgets/QLabel \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtWidgets/QDockWidget \
		qcnlive_define.h \
		myframe.h
	/Developer/Qt5.0.0/5.0.0/clang_64/bin/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__=4 myframe.h -o moc_myframe.cpp

moc_dlgsettings.cpp: qcnqt.h \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtWidgets/QApplication \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtGui/QtGui \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtOpenGL/QtOpenGL \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtWidgets/QMainWindow \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtWidgets/QSplashScreen \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtCore/QTimer \
		../main/main.h \
		../main/define.h \
		../../../qcn/config.h.win \
		../../../qcn/config.h \
		../../../boinc/api/boinc_gl.h \
		../util/qcn_util.h \
		../main/qcn_shmem.h \
		../util/sac.h \
		../util/qcn_thread.h \
		../main/qcn_thread_time.h \
		../main/qcn_thread_sensor.h \
		../util/qcn_signal.h \
		../util/execproc.h \
		../../../boinc/lib/boinc_win.h \
		../sensor/csensor.h \
		../../../boinc/api/boinc_api.h \
		../../../boinc/lib/diagnostics.h \
		../../../boinc/lib/str_util.h \
		../../../boinc/lib/filesys.h \
		../../../boinc/lib/parse.h \
		../../../boinc/lib/mfile.h \
		../../../boinc/api/graphics2.h \
		../../../boinc/lib/util.h \
		../util/qcn_gzip.h \
		../util/trickleup.h \
		../util/trickledown.h \
		../graphics/qcn_graphics.h \
		../util/qcn_curl.h \
		myframe.h \
		glwidget.h \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtOpenGL/QGLWidget \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtWidgets/QSlider \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtWidgets/QScrollArea \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtWidgets/QLabel \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtWidgets/QDockWidget \
		qcnlive_define.h \
		dlgsettings.h
	/Developer/Qt5.0.0/5.0.0/clang_64/bin/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__=4 dlgsettings.h -o moc_dlgsettings.cpp

moc_dlgmakequake.cpp: qcnqt.h \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtWidgets/QApplication \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtGui/QtGui \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtOpenGL/QtOpenGL \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtWidgets/QMainWindow \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtWidgets/QSplashScreen \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtCore/QTimer \
		../main/main.h \
		../main/define.h \
		../../../qcn/config.h.win \
		../../../qcn/config.h \
		../../../boinc/api/boinc_gl.h \
		../util/qcn_util.h \
		../main/qcn_shmem.h \
		../util/sac.h \
		../util/qcn_thread.h \
		../main/qcn_thread_time.h \
		../main/qcn_thread_sensor.h \
		../util/qcn_signal.h \
		../util/execproc.h \
		../../../boinc/lib/boinc_win.h \
		../sensor/csensor.h \
		../../../boinc/api/boinc_api.h \
		../../../boinc/lib/diagnostics.h \
		../../../boinc/lib/str_util.h \
		../../../boinc/lib/filesys.h \
		../../../boinc/lib/parse.h \
		../../../boinc/lib/mfile.h \
		../../../boinc/api/graphics2.h \
		../../../boinc/lib/util.h \
		../util/qcn_gzip.h \
		../util/trickleup.h \
		../util/trickledown.h \
		../graphics/qcn_graphics.h \
		../util/qcn_curl.h \
		myframe.h \
		glwidget.h \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtOpenGL/QGLWidget \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtWidgets/QSlider \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtWidgets/QScrollArea \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtWidgets/QLabel \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtWidgets/QDockWidget \
		qcnlive_define.h \
		/Developer/Qt5.0.0/5.0.0/clang_64/include/QtCore/QList \
		dlgmakequake.h
	/Developer/Qt5.0.0/5.0.0/clang_64/bin/moc $(DEFINES) $(INCPATH) -D__APPLE__ -D__GNUC__=4 dlgmakequake.h -o moc_dlgmakequake.cpp

compiler_moc_source_make_all:
compiler_moc_source_clean:
compiler_uic_make_all:
compiler_uic_clean:
compiler_rez_source_make_all:
compiler_rez_source_clean:
compiler_yacc_decl_make_all:
compiler_yacc_decl_clean:
compiler_yacc_impl_make_all:
compiler_yacc_impl_clean:
compiler_lex_make_all:
compiler_lex_clean:
compiler_clean: compiler_moc_header_clean 

