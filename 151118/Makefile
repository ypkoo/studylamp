#PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:${PKG_CONFIG_PATH}
#export PKG_CONFIG_PATH

DEBUG ?= 1
ifeq (DEBUG, 1)
    CFLAGS=-DDEBUG
endif

all:
	g++ -o studyLamp main.cpp gesture.cpp detector.cpp -llept -ltesseract `pkg-config --cflags --libs opencv` $(FLAG)
debug:
	g++ -o studyLamp main.cpp gesture.cpp detector.cpp -llept -ltesseract `pkg-config --cflags --libs opencv` $(FLAG)
debug: FLAG=-DNDEBUG

