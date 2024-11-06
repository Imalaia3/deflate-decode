CXX=g++
CPPFLAGS=-g -Wall -Wextra

SRCS=deflate.cpp huffman.cpp main.cpp
OBJS=$(patsubst %.cpp,bin/%.o,$(SRCS))

all: test

test: $(OBJS)
	$(CXX) $(CPPFLAGS) -o bin/test $(OBJS)

bin/%.o: %.cpp
	mkdir -p bin
	$(CXX) $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf bin/*.*

