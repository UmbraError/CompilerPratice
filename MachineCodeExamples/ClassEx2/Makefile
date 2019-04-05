CXX         = clang++
LIBARYFLAGS = 
CXXFLAGS    = -std=c++1z -Wall -Wextra -Wparentheses -g

.PHONY:all
all: gen3.bin

gen3.bin: gen3.o
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $^

.PHONY: clean
clean:
	rm -f *.o *.bin
