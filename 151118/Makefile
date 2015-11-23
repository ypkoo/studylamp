#PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:${PKG_CONFIG_PATH}
#export PKG_CONFIG_PATH

all:
	g++ -o studyLamp main.cpp gesture.cpp imageView.cpp detector.cpp -llept -ltesseract `pkg-config --cflags --libs opencv` $(FLAG)
debug:
	g++ -o studyLamp main.cpp gesture.cpp imageView.cpp detector.cpp -llept -ltesseract `pkg-config --cflags --libs opencv` $(FLAG)
debug: FLAG=-DNDEBUG
