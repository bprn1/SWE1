CC = gcc
CFLAGS = -Wall -Wextra -g
LIBS = -lncurses -lzmq -ljansson
SRCS = ui.c 
OBJS = $(SRCS:.c=.o)
TARGET = game

all: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

%.o: %.c ui.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: all
	./$(TARGET)