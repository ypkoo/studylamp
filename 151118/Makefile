#PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:${PKG_CONFIG_PATH}
#export PKG_CONFIG_PATH

DEBUG ?= 1
ifeq (DEBUG, 1)
    CFLAGS=-DDEBUG
endif

all:
	g++ -o studyLamp main.cpp trackFinger.cpp gesture.cpp imageView.cpp  `pkg-config --cflags --libs opencv` $(CFLAGS)
