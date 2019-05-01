#include <iostream>
using std::cout;
using std::dec;
using std::endl;
using std::hex;

#include <errno.h>
#include <sys/mman.h>

void print(long int i) { cout << "Value: " << i << endl; }

int main() {
	char* program;
	int p_offset = 0;

	program = (char*)mmap(0, 50000, PROT_EXEC | PROT_READ | PROT_WRITE,
	                      MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);

	if (errno) {
		perror("mmap: ");
		return 0;
	}

	program[p_offset++] = '\xb8';  // Move immediate 32 bit to EAX
	program[p_offset++] = '\x03';
	program[p_offset++] = '\x00';
	program[p_offset++] = '\x00';
	program[p_offset++] = '\x00';
	program[p_offset++] = '\xb9';  // Move immediate 32 bit to ECX
	program[p_offset++] = '\x04';
	program[p_offset++] = '\x00';
	program[p_offset++] = '\x00';
	program[p_offset++] = '\x00';
	//  program[p_offset++] = '\x91';  // XCHG EAX, ECX
	//  program[p_offset++] = '\x87';
	//  program[p_offset++] = '\xc1';
	program[p_offset++] = '\x01';  // ADD EAX, ECX
	program[p_offset++] = '\xc8';
	program[p_offset++] =
	    '\x89';  // MOV RDI, RAX  -- ABI specifies use of RDI
	program[p_offset++] =
	    '\xc7';  //               -- for passing a single int
	program[p_offset++] = '\x48';  // MOV RSI, &print
	program[p_offset++] = '\xbe';

	long long int func_loc = (long long int)print;
	cout << "print(): " << hex << (long long int)print << dec << endl;
	// This is an ugly way of generating the location.
	// A better way would be to create a function that generates the code
	program[p_offset++] = func_loc & 0xff;
	func_loc >>= 8;
	program[p_offset++] = func_loc & 0xff;
	func_loc >>= 8;
	program[p_offset++] = func_loc & 0xff;
	func_loc >>= 8;
	program[p_offset++] = func_loc & 0xff;
	func_loc >>= 8;
	program[p_offset++] = func_loc & 0xff;
	func_loc >>= 8;
	program[p_offset++] = func_loc & 0xff;
	func_loc >>= 8;
	program[p_offset++] = func_loc & 0xff;
	func_loc >>= 8;
	program[p_offset++] = func_loc & 0xff;
	program[p_offset++] = '\xff';  // CALL RSI
	program[p_offset++] = '\xd6';

	program[p_offset++] = '\xc3';  // Return

	// Call here
	int value = 0;

	value = ((int (*)())(program))();

	cout << "Success!" << endl;
	cout << "value: " << value << endl;

	munmap(program, 50000);

	return 0;
}
