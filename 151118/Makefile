#PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:${PKG_CONFIG_PATH}
#export PKG_CONFIG_PATH

all:
	g++ -o opencv main.cpp trackFinger.cpp gesture.cpp `pkg-config —cflags —libs opencv`
