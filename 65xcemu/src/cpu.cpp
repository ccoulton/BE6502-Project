#include "../inc/typedef.h"
#include "../inc/AddressSpace.h"
#include "../inc/6502cpu.h"

int main() {
  AddressSpace program;
  CPU cpu(program);
  cpu.run(4);
  return 0;
}
