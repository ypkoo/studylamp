@ECHO OFF
SET OPENCV_DIR=C:\opencv\
SET TESSERACT_DIR=C:\tesseract\
SET LEPT_DIR=C:\leptonica\

SET CPP_FILES=main.cpp gesture.cpp messenger.cpp detector.cpp buttonDetector.cpp
SET LIBS=
SET INCLUDE_FOLDERS=
SET LIB_PATH=

:: WINDOW
SET LIBS=%LIBS% gdi32.lib User32.lib Advapi32.lib comdlg32.lib Ole32.lib vfw32.lib comctl32.lib OleAut32.lib Ws2_32.lib

:: OPENCV
SET LIBS=%LIBS% IlmImf.lib IlmImfd.lib ippicvmt.lib libjasper.lib libjasperd.lib libjpeg.lib libjpegd.lib libpng.lib libpngd.lib libtiff.lib libtiffd.lib libwebp.lib libwebpd.lib opencv_calib3d300.lib opencv_calib3d300d.lib opencv_core300.lib opencv_core300d.lib opencv_features2d300.lib opencv_features2d300d.lib opencv_flann300.lib opencv_flann300d.lib opencv_hal300.lib opencv_hal300d.lib opencv_highgui300.lib opencv_highgui300d.lib opencv_imgcodecs300.lib opencv_imgcodecs300d.lib opencv_imgproc300.lib opencv_imgproc300d.lib opencv_ml300.lib opencv_ml300d.lib opencv_objdetect300.lib opencv_objdetect300d.lib opencv_photo300.lib opencv_photo300d.lib opencv_shape300.lib opencv_shape300d.lib opencv_stitching300.lib opencv_stitching300d.lib opencv_superres300.lib opencv_superres300d.lib opencv_ts300.lib opencv_ts300d.lib opencv_video300.lib opencv_video300d.lib opencv_videoio300.lib opencv_videoio300d.lib opencv_videostab300.lib opencv_videostab300d.lib zlib.lib zlibd.lib
SET INCLUDE_FOLDERS=%INCLUDE_FOLDERS% /I%OPENCV_DIR%sources\3rdparty\include /I%OPENCV_DIR%sources\include /I%OPENCV_DIR%sources\modules\calib3d\include /I%OPENCV_DIR%sources\modules\core\include /I%OPENCV_DIR%sources\modules\cudaarithm\include /I%OPENCV_DIR%sources\modules\cudabgsegm\include /I%OPENCV_DIR%sources\modules\cudacodec\include /I%OPENCV_DIR%sources\modules\cudafeatures2d\include /I%OPENCV_DIR%sources\modules\cudafilters\include /I%OPENCV_DIR%sources\modules\cudaimgproc\include /I%OPENCV_DIR%sources\modules\cudalegacy\include /I%OPENCV_DIR%sources\modules\cudaobjdetect\include /I%OPENCV_DIR%sources\modules\cudaoptflow\include /I%OPENCV_DIR%sources\modules\cudastereo\include /I%OPENCV_DIR%sources\modules\cudawarping\include /I%OPENCV_DIR%sources\modules\cudev\include /I%OPENCV_DIR%sources\modules\features2d\include /I%OPENCV_DIR%sources\modules\flann\include /I%OPENCV_DIR%sources\modules\hal\include /I%OPENCV_DIR%sources\modules\highgui\include /I%OPENCV_DIR%sources\modules\imgcodecs\include /I%OPENCV_DIR%sources\modules\imgproc\include /I%OPENCV_DIR%sources\modules\ml\include /I%OPENCV_DIR%sources\modules\objdetect\include /I%OPENCV_DIR%sources\modules\photo\include /I%OPENCV_DIR%sources\modules\shape\include /I%OPENCV_DIR%sources\modules\stitching\include /I%OPENCV_DIR%sources\modules\superres\include /I%OPENCV_DIR%sources\modules\ts\include /I%OPENCV_DIR%sources\modules\video\include /I%OPENCV_DIR%sources\modules\videoio\include /I%OPENCV_DIR%sources\modules\videostab\include /I%OPENCV_DIR%sources\modules\viz\include /I%OPENCV_DIR%sources\modules\world\include
SET LIB_PATH=%LIB_PATH% /LIBPATH:%OPENCV_DIR%build\x86\vc12\staticlib

:: TESSERACT
SET LIBS=%LIBS% libtesseract302.lib
SET INCLUDE_FOLDERS=%INCLUDE_FOLDERS% /I%TESSERACT_DIR%\include
SET LIB_PATH=%LIB_PATH% /LIBPATH:%TESSERACT_DIR%\lib

:: LEPT
SET LIBS=%LIBS% giflib416-static-mtdll.lib libjpeg8c-static-mtdll.lib liblept168-static-mtdll.lib libpng143-static-mtdll.lib libtiff394-static-mtdll.lib zlib125-static-mtdll.lib
SET INCLUDE_FOLDERS=%INCLUDE_FOLDERS% /I%LEPT_DIR%\include
SET LIB_PATH=%LIB_PATH% /LIBPATH:%LEPT_DIR%\lib

:: pHash
SET LIBS=%LIBS% pHash.lib
SET INCLUDE_FOLDERS=%INCLUDE_FOLDERS% /IC:\Workspace\pHash-0.9.4
SET LIB_PATH=%LIB_PATH% /LIBPATH:C:\Workspace\pHash-0.9.4\Release

cl /DDEBUG /FestudyLamp /MT /GA /O2 /EHsc %INCLUDE_FOLDERS% %CPP_FILES% %LIBS% /link %LIB_PATH% /NODEFAULTLIB:libcmt.lib /NODEFAULTLIB:msvcprt.lib

@ECHO ON