export CXX = g++
export CXXFLAGS = -I$(shell pwd) -g -fvisibility=hidden -Wall -Wextra -Wpedantic -Wconversion -Wno-sign-conversion -Wno-missing-field-initializers #-std=c++17 #-Werror
LDFLAGS= -pthread

TARGET = msub

SRCS = main.cpp msub.cpp

OBJS = $(SRCS:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

format:
	./format.sh

db:
	rm -rf build/compile_commands; mkdir -p build; $(MAKE) clean
	bear --output build/compile_commands.json --append -- $(MAKE)

typos:
	typos . --exclude "json.hpp" --exclude "httplib.h" --exclude "tests/test.cpp"

clean:
	rm -f $(TARGET) $(OBJS)

.PHONY: help all format typos clean
help:
	@echo "Usage:"
	@echo "  make            Build the project"
	@echo "  make clean      Clean the project"
	@echo "  make help       Show this help message"
