CC = gcc
CFLAGS = -Wall -Wextra -g
LIBS = -lncurses
SRCS = ui.c curses.c
OBJS = $(SRCS:.c=.o)
TARGET = game

all: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

run: all
	./$(TARGET)