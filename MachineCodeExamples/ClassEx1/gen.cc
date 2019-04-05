#include <iostream>
using std::cout;
using std::endl;

#include <sys/mman.h>
#include <errno.h>

int main()
{
  char * program;
  int p_offset = 0;

  program = (char *)  mmap(0, 50000, PROT_EXEC | PROT_READ | PROT_WRITE,
			   MAP_PRIVATE | MAP_ANONYMOUS,
			   -1, 0);

  cout << "memory at " << (int *) program << endl;
  
  if(errno)
    {
      perror("mmap");
      return 0;
    }

  // Load up code
  program[p_offset++] = '\xc3';  // Return

  

  // Call here

  ((int(*)())(program))(); // Honest, this calls the function
  
  cout << "Success!" << endl;
    
  munmap(program, 50000);

return 0;
}
