CROSS_COMPILE := arm-none-eabi-
CC := gcc
AR := ar

CCFLAGS	:= -Wno-packed-bitfield-compat \
	   -fverbose-asm -fstrict-volatile-bitfields \
	   -fno-builtin-fprintf -fno-builtin-printf \
	   -fdata-sections -ffunction-sections \
	   -std=c99 -c \
	   -Wall -Werror \
	   -Os
INCS := -I.
SRCS := cli.c
OBJS := $(patsubst %.c,%.o,$(SRCS))
TARGET := libcli.a

.PHONY:all clean install

all: $(TARGET) 
$(TARGET):$(OBJS)
	$(CROSS_COMPILE)$(AR) crs $@ $^

%.o:%.c
	$(CROSS_COMPILE)$(CC) $(CCFLAGS) $(INCS) -c $^ -o $@

clean:
	@echo "clean"
	@rm -rf $(TARGET)
	@rm -rf $(OBJS)

