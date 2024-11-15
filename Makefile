CXX=g++
CPPFLAGS=-g -Wall -Wextra -Wpedantic

TEST_SRCS=src/deflate/deflate.cpp src/deflate/huffman.cpp src/test.cpp
TEST_OBJS=$(patsubst %.cpp,bin/%.o,$(TEST_SRCS))

DEMO_SRCS=src/deflate/deflate.cpp src/deflate/huffman.cpp src/demo.cpp
DEMO_OBJS=$(patsubst %.cpp,bin/%.o,$(DEMO_SRCS))

all: test demo

test: $(TEST_OBJS)
	$(CXX) $(CPPFLAGS) -o bin/test $(TEST_OBJS)

demo: $(DEMO_OBJS)
	$(CXX) $(CPPFLAGS) -o bin/demo $(DEMO_OBJS)

bin/%.o: %.cpp
	mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) -c $< -o $@

clean:
	rm -rf bin/

