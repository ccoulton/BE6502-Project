#include <fstream>

#include "../inc/typedef.h"
#include "../inc/AddressSpace.h"
#include "../inc/6502cpu.h"

int main() {
  AddressSpace program(0x8000);
  program.loadTest();
  CPU cpu(program);
  cpu.run(17);
  return 0;
}
