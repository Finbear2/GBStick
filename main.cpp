#include "memory.h"
#include "cpu.h"
#include <iostream>

int main(int argc, char *argv[]) {

  if (argc < 2) {
    std::cerr << "Usage: ./gameboy <rom.gb>\n";
    return 1;
  }

  Memory memory;

  if (!memory.loadRom(argv[1])) {
    return 1;
  }

  CPU cpu(memory);

  bool success = cpu.step();

  while (success != false) {
    success = cpu.step();
  }

  return 0;
}