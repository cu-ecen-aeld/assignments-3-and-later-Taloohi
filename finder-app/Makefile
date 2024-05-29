# writer.o : aarch64-none-linux-gnu-gcc -g -Wall -c -o writer.o writer.c
CC = $(CROSS_COMPILE)gcc
SRC = writer.c
FILES = $(SRC:.c=.o)

all: writer

# writer: $(FILES) $(CC) -o $(@) $(FILES)

writer: $(FILES)
	 $(CC) -o $(@) $(FILES)

clean:
	rm -rf writer $(FILES)

ifdef CROSS_COMPILE
CC := $(CROSS_COMPILE)gcc -g -Wall
endif
