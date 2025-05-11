CC = gcc
CFLAGS = -Wall -Wextra -std=c99

SRCS = main.c auth.c booking.c lostfound.c admin.c helpers.c validation.c
OBJS = $(SRCS:.c=.o)
TARGET = cms

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c headers.h
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f $(OBJS) $(TARGET)