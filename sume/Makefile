########################
#
#Liang 
#CS MTU
#Project 2
########################

# the compiler: gcc for C program, define as g++ for C++
CC = gcc

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
#CFLAGS  = -g -Wall
CFLAGS  = -c -g 

# the build target executable:
TARGET = sume
SRC = $(wildcard *.c)
OBJ = $(patsubst %.c,%.o,$(SRC))
.PHONY: clean


all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ 

 %o: %c
	$(CC) $(CFLAGS) -o $@ $<

#make run
run:
	./$(TARGET) ${ARGS}

# make clean
clean:
	$(RM) $(TARGET) $(OBJ)
