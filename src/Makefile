
CROSS = 
CC = $(CROSS)gcc
CXX = $(CROSS)g++
MV = mv -f
RM = rm -rf
LN = ln -sf

DEBUG = 
#CFLAGS = $(DEBUG) -Wall -c
CFLAGS = $(DEBUG) 
#SVN_REVISION = $(shell svn info |grep Revision|awk '{print $$2}') 
REVISION = $(shell git rev-parse HEAD)
CFLAGS  += -DREVISION="\"$(REVISION)"\"

TARGET = http_agent
 
TOP_PATH = $(shell pwd)
SRC_PATH = $(TOP_PATH)/src
MOD_PATH = $(TOP_PATH)/modules
MOD_INC_PATH = $(MOD_PATH)/include

LIB_PATH = $(TOP_PATH)/libs
LIB_INC_PATH = $(TOP_PATH)/libs/include

DIRS = $(shell find $(SRC_PATH) -maxdepth 3 -type d | grep -v .git)
FILES = $(foreach dir, $(DIRS), $(wildcard $(dir)/*.c))


##########################################################
# libs
##########################################################
#libs = $(shell find $(LIB_PATH) -maxdepth 1 -type d | grep -v .git)
libs = $(shell find $(LIB_PATH) -mindepth 1 -maxdepth 1 -type d | grep -v .git | grep -v include)
#libs = $(shell find $(LIB_PATH) -mindepth 1 -maxdepth 1 -type d )

LIBS_PATH = $(foreach lb, $(libs), $(lb))

 
##########################################################
# modules
##########################################################
modules = $(shell find $(MOD_PATH) -mindepth 1 -maxdepth 1 -type d | grep -v .git | include)
MODULES_PATH = $(foreach m, $(modules), $(MOD_PATH)/$(m))
 
##########################################################
# srcs
##########################################################
#SRCS_CPP += $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cpp))
#SRCS_CC += $(foreach dir, $(DIRS), $(wildcard $(dir)/*.cc))
SRCS_C += $(foreach dir, $(DIRS), $(wildcard $(dir)/*.c))
 
##########################################################
# objs
##########################################################
#OBJS_CPP = $(patsubst %.cpp, %.o, $(SRCS_CPP))
#OBJS_CC = $(patsubst %.cc, %.o, $(SRCS_CC))
OBJS_C = $(patsubst %.c, %.o, $(SRCS_C))
 
##########################################################
# paths
##########################################################
INC_PATH += -I$(TOP_PATH)/include
INC_PATH += -I$(LIB_INC_PATH)
INC_PATH += -I$(SRC_PATH)
INC_PATH += -I$(MOD_INC_PATH)

LD_LIB += -L$(LIB_PATH)
 
##########################################################
# libs
##########################################################
 
##########################################################
# building
##########################################################
all:$(TARGET)
 
$(TARGET) : obj_libs $(OBJS_C) 
	@ for i in $(MODULES_PATH); \
	do \
	make -C $$i; \
	done
 
	@ $(CC) $^ -o $@ $(LD_LIB) $(LIBS)
	@ echo Create $(TARGET) ok...

obj_libs:
	@echo $(libs)
	for dir in $(libs); \
	do \
	echo $$(dir)\
	make -C $$dir; \
	done
	@ echo  Create $$(dir) : libs ok...

$(OBJS_C):%.o : %.c
	$(CC) $(CFLAGS) $< -o $@ $(INC_PATH)
 
.PHONY : clean
clean:
	@ $(RM) $(TARGET) $(OBJS_C)
	@ for i in $(LIBS_PATH); \
	do \
	make clean -C $$i; \
	done
	@ for i in $(MODULES_PATH); \
	do \
	make clean -C $$i; \
	done