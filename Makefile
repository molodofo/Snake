SRC_FILE = snake.c
APPOUT = snake
CC = gcc
C_FLAGS = -Wall -pthread 

all:
	$(CC) $(C_FLAGS) $(SRC_FILE) -o $(APPOUT)
	
clean:
	rm $(APPOUT)
