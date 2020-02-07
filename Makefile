CC = g++
FLAGS = -Werror -I /usr/include/SDL2 
TARGET = output
EXEC = *.cpp
INPUT = ""
OUTPUT = ""

default: main.o

main.o: $(EXEC)
	$(CC) $(FLAGS) $(EXEC) -o $(TARGET) -lSDL2

run: main.o
	./$(TARGET) 
	-rm $(TARGET)

format:
	cp /home/hosni/Projects/C++/Style/.clang-format .clang-format
	clang-format -i *.cpp -style=file
