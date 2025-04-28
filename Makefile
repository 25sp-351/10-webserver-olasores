CC = gcc
CFLAGS = -Wall -Wextra -g -pthread
LDFLAGS = -pthread

SOURCES = main.c server.c request.c response.c route_handlers.c utils.c
OBJECTS = $(SOURCES:.c=.o)
EXECUTABLE = http_server

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.PHONY: all clean
