#pragma once

#include "memory.h"
#include "registers.h"
#include <cstdint>
#include <iostream>

class CPU {
public:
  CPU(Memory &memory);

  bool step();

private:
  Memory &memory;
  bool ime = false;
  int cycles = 0;
  int targetCycles = 0;
  bool stopped = false;

  uint8_t fetch8();
  uint16_t fetch16();
};