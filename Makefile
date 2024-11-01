CC = gcc

#CFLAGS = -g -Wall -std=c99

TARGET = main
SOURCES = main.c 
OBJECTS = $(SOURCES:.c=.o)

$(TARGET) : $(OBJECTS)
	$(CC) -o $@ $^

run: $(TARGET)
	./$(TARGET)

.PHONY: clean

clean:
	@rm -f $(TARGET) $(OBJECTS) core
