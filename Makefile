CFLAGS += $(COMMON_CFLAGS) -lm 

TARGET=agent
DEPEND=$(TARGET).dep
SRC=$(wildcard *.c)
OBJ=$(patsubst %.c, %.o, $(SRC))

LIB=data_tree.so

all:
	make dep-$(DEPEND)
	make tag-$(TARGET)

dep-$(DEPEND):
	-rm -rf $(DEPEND)
	for file in $(SRC); do \
		$(CC) $(CFLAGS) -MM $${file} >> $(DEPEND); \
	done

tag-$(TARGET): $(OBJ) $(LIB)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJ) $(LIB)

clean:
	-rm -rf *.o $(TARGET) $(DEPEND)

ifneq ($(wildcard $(DEPEND)),$(empty))
include $(wildcard $(DEPEND))
endif

romfs:
	$(ROMFSINST) $(TARGET) /bin/$(TARGET)
