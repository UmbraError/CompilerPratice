CXX         = clang++
LIBARYFLAGS = 
CXXFLAGS    = -std=c++1z -Wall -Wextra -Wparentheses -g

.PHONY:all
all: gen.bin

gen.bin: gen.o
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $^

.PHONY: clean
clean:
	rm -f *.o *.bin
