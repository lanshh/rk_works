BIN =emmc_startup
CC = ../../prebuilts/toolschain/usr/bin/arm-linux-gcc
CPP = ../../prebuilts/toolschain/usr/bin/arm-linux-g++
INCS = -I./../../out/system/include/ -I./
LIBS = -L./../../out/system/lib -lm -lpthread -lz
ETCPATH  = ../../out/system/etc
RESPATH  = ../../out/system/share/minigui/res/images
RELEASEDIR = ../../out/target/bin

define all-cpp-files-under
$(shell find $(1) -name "*."$(2) -and -not -name ".*" )
endef

define all-subdir-cpp-files
$(call all-cpp-files-under,.,"cpp")
endef

CPPSRCS = $(call all-subdir-cpp-files)
CSRCS = $(wildcard *.c)


COBJS := $(CSRCS:.c=.o)
CPPOBJS := $(CPPSRCS:.cpp=.o)

ADB_COBJS := $(CSRCS:.c=.o)
ADB_CPPOBJS := $(CPPSRCS:.cpp=.o)

DEBUGFLAGS = -fexceptions -finstrument-functions -funwind-tables -g -rdynamic -O0 -DDEBUG
#DEBUGFLAGS += -DSTRESS_TEST
#DEBUGFLAGS += -DMUL_ENCODERS_DEBUG
OPTIMIZEFLAGS = -O3 -Wl,-gc-sections
LOCAL_CPPFLAGS += -DUSE_RK_DISPLAY -DSUPPORT_ION
CFLAGS = -Wno-multichar $(LOCAL_CPPFLAGS)
CPPFLAGS = -DSUPPORT_ION -std=c++11 -DLINUX  -DENABLE_ASSERT -Wno-multichar -DResolution -Wall $(LOCAL_CPPFLAGS)

BINPATH  = ../../out/root/bin

all : $(BIN)

$(COBJS) : %.o : %.c
	@$(CC) -c $< -o $@ $(INCS) $(CFLAGS) $(LIBS)
$(CPPOBJS) : %.o : %.cpp
	@$(CPP) -c $< -o $@ $(INCS) $(CPPFLAGS) $(LIBS)

$(BIN) : $(COBJS) $(CPPOBJS)
	@$(CPP) -o $(BIN) $(COBJS) $(CPPOBJS) $(LIBS)
	@-rm $(COBJS) $(CPPOBJS)
	@echo "$(BIN) build success!"
	@cp -r $(BIN) $(RELEASEDIR)

clean:
	@rm -f $(BIN) $(COBJS) $(CPPOBJS)
	@rm enable_adb
	@echo "$(BIN) enable_adb clean success!"


ADB_COBJS:
	@$(CC) -c adb/enable_adb_main.c usb_ctrl.c $(INCS) $(CFLAGS) 


enable_adb:ADB_COBJS
	@$(CPP) -o enable_adb enable_adb_main.o usb_ctrl.o
	@cp -r enable_adb $(BINPATH)
	@rm enable_adb_main.o usb_ctrl.o
	@echo "enable_adb build success!"
