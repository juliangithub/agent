CFLAGS += $(COMMON_CFLAGS) -lm -DCONFIG_SKY_ALARM

TARGET=sky_alarm
DEPEND=$(TARGET).dep
SRC=$(wildcard *.c)
OBJ=$(patsubst %.c, %.o, $(SRC))

APMIBLIB=../boa/apmib/libapmib.so

all:
	make dep-$(DEPEND)
	make tag-$(TARGET)

dep-$(DEPEND):
	-rm -rf $(DEPEND)
	for file in $(SRC); do \
		$(CC) $(CFLAGS) -MM $${file} >> $(DEPEND); \
	done

tag-$(TARGET): $(OBJ) $(APMIBLIB)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(APMIBLIB)

clean:
	-rm -rf *.o $(TARGET) $(DEPEND)

ifneq ($(wildcard $(DEPEND)),$(empty))
include $(wildcard $(DEPEND))
endif

romfs:
	$(ROMFSINST) $(TARGET) /bin/$(TARGET)
