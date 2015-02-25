# the compiler: gcc for C program, define as g++ for C++
CC = gcc

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -g -Wall

# the build target executable:
TARGET = traverse
  
all: $(TARGET)
  
$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c

#make run
run:
	./$(TARGET) ${ARGS}

# make clean
.PHONY: clean
clean:
	$(RM) $(TARGET)
