CXX         = clang++
LIBARYFLAGS = 
CXXFLAGS    = -std=c++1z -Wall -Wextra -Wparentheses -g

.PHONY:all
all: compilerMain.bin

compilerMain.bin: compilerMain.o lexicalAnalyzer.o token.o
	$(CXX) $(CXXFLAGS) -o $@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $^

.PHONY: clean
clean:
	rm -f *.o *.bin
