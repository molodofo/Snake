SRC_FILE = snake.c a_star.c find_food.c -lm 
APPOUT = snake
CC = gcc
C_FLAGS = -g -Wall -pthread 

all:
	$(CC) $(C_FLAGS) $(SRC_FILE) -o $(APPOUT)
	
clean:
	rm $(APPOUT)
