CXX=g++
CPPFLAGS=-g -Wall -Wextra

TEST_SRCS=src/deflate/deflate.cpp src/deflate/huffman.cpp src/test.cpp
TEST_OBJS=$(patsubst %.cpp,bin/%.o,$(TEST_SRCS))

all: test

test: $(TEST_OBJS)
	$(CXX) $(CPPFLAGS) -o bin/test $(TEST_OBJS)

bin/%.o: %.cpp
	mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf bin/

