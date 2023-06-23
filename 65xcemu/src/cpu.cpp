#include <fstream>

#include "../inc/typedef.h"
#include "../inc/AddressSpace.h"
#include "../inc/6502cpu.h"

int main() {
  std::ifstream inputfile;
  inputfile.open("", std::ios_base::binary);
  AddressSpace program(0x8000);
  CPU cpu(program);
  cpu.run(4);
  return 0;
}
