X86_VERSION=y

ifeq ($(X86_VERSION),y)
CC := gcc
LD := ld
CFLAGS  = -Wall -g -O0
#CFLAGS  += -DX86_VERSION
CFLAGS += -I./lib
#LDFLAGS += -lcurl
TARGET=x86_bin
else
CC = arm-linux-gcc
LD = arm-linux-ld
CFLAGS  = -Wall -g -O0
CFLAGS += -I./include/curl
CFLAGS +=  -I.
#CFLAGS += -L./lib -lcurl
LDFLAGS =
TARGET=arm_bin
endif

#SVN_REVISION = $(shell svn info |grep Revision|awk '{print $$2}') 
REVISION = $(shell git rev-parse HEAD)
CFLAGS  += -DREVISION="\"$(REVISION)"\"

TOP_PATH = $(shell pwd)
#SRC_PATH = $(TOP_PATH)/src
SRC_PATH = $(TOP_PATH)

LIB_PATH = $(TOP_PATH)/lib
DIRS=$(shell find)

SRC=$(wildcard *.c)
OBJS=$(patsubst %.c, %.o, $(SRC))

.PHONY: $(TARGET)

$(TARGET) : $(OBJS)
	echo $(REVISION)
	$(CC) -o $@ $^ $(CFLAGS) $(LDFLAGS)
	echo " -> Compiling ok."


clean:
	-@rm $(OBJS)
	-@rm $(TARGET)
	
