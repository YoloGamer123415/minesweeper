CC := g++
CFLAGS := -Wall -g
LIBS := $(shell pkg-config --cflags --libs ncurses)
TARGET := minesweeper

# $(wildcard *.cpp /xxx/xxx/*.cpp): get all .cpp files from the current directory and dir "/xxx/xxx/"
SRCS := $(wildcard src/*.cpp src/**/*.cpp)
# $(patsubst %.cpp,%.o,$(SRCS)): substitute all ".cpp" file name strings to ".o" file name strings
OBJS := $(patsubst %.cpp,%.o,$(SRCS))

all: $(TARGET)
$(TARGET): $(OBJS)
	$(CC) -o $@ $^ $(LIBS)

%.o: %.cpp
	$(CC) $(CFLAGS) -c $< -o $@ $(LIBS)

clean:
	rm -rf $(TARGET) src/*.o src/**/*.o
