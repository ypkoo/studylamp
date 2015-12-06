#PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:${PKG_CONFIG_PATH}
#export PKG_CONFIG_PATH

CC = g++
MYFLAG = -llept -ltesseract `pkg-config --cflags --libs opencv`
ifeq ($(DEBUG), 0)
MYFLAG += -DNDEBUG
else
MYFLAG += -DDEBUG
endif

SRCS = main.cpp gesture.cpp detector.cpp settingLoader.cpp buttonDetector.cpp
TARGET = studyLamp

all:
	$(CC) -o $(TARGET) $(SRCS) $(MYFLAG)