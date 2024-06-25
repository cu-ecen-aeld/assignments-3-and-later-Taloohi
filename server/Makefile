SRC := aesdsocket.c
TARGET = aesdsocket
OBJS := $(SRC:.c=.o)
CC=$(CROSS_COMPILE)gcc
CFLAGS = -Wall -Wextra -I.
all: $(TARGET)

$(TARGET) : $(OBJS)
# $(TARGET) : writer.c
	$(CC) $(CFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	-rm -f *.o $(TARGET) *.elf *.map