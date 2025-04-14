CC = gcc
CFLAGS = -Wall -g
LIBS = -lsqlite3
SRC = main.c
TARGET = app

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRC) $(LIBS)

clean:
	rm -f $(TARGET)
