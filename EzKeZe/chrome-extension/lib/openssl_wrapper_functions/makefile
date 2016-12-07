CC = gcc

CFLAGS = -g -Wall -I include/ -L lib -lssl -lcrypto -ldl 

TARGET = encrypt

all: clean $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(TARGET).c -o $(TARGET) $(CFLAGS)

clean:
	$(RM) $(TARGET)

lib: clean $(TARGET).c
	$(CC) -c $(TARGET).c  $(CFLAGS)
