#!/usr/bin/env bash
if [ "$JOB" == "5.7.0" ]
	then
		wget -q "http://download.qt.io/archive/qt/5.7/5.7.0/submodules/qtbase-opensource-src-5.7.0.tar.xz"
		tar -xvf qtbase-opensource-src-5.7.0.tar.xz
		cd qtbase-opensource-src-5.7.0
		./configure -shared -release -silent -opensource -confirm-license -opengl -nomake examples -qt-xcb -sm -qt-libpng -no-libjpeg -no-icu -qt-zlib -qt-pcre
		make -j$(nproc)
		make install
		mkdir build
		/usr/local/Qt-5.7.0/bin/qmake ..
		make -j$(nproc)
		wget https://raw.githubusercontent.com/probonopd/uploadtool/master/upload.sh
		export UPLOADTOOL_SUFFIX="Qt$JOB"
		bash upload.sh ./AppImageUpdaterBridge/libAppImageUpdaterBridge.so
fi


