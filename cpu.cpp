#include "cpu.h"
#include "registers.h"
#include <iostream>

CPU::CPU(Memory &memory) : memory(memory) {}

uint8_t CPU::fetch8() {
  uint8_t value = memory.read(registers.pc);
  registers.pc++;

  return value;
}

uint16_t CPU::fetch16() {

  uint8_t low = fetch8();
  uint8_t high = fetch8();

  return (high << 8) | low;
}

bool CPU::step() {

  uint8_t instruction = fetch8();

  if (instruction == 0xCB) {
    uint8_t cbInstruction = fetch8();

    switch (cbInstruction) {
    default:
      std::cerr << "Unknown CB instruction: 0x" << std::hex
                << (int)cbInstruction << '\n';
      break;
    }

    return false;
  }

  // std::cout << instruction;

  uint16_t previousPC = registers.pc;

  std::cout << "PC: 0x" << std::hex << previousPC << " | OP: 0x"
            << (int)instruction << " | A: 0x" << (int)registers.a << " | F: 0x"
            << (int)registers.f << '\n';

  std::cout << std::hex << "PC: 0x" << registers.pc - 1 << " OP: 0x"
            << (int)instruction << '\n';

  switch (instruction) {

  case 0x00: // NOP
    break;

  case 0x3E: // LD A, d8
    registers.a = fetch8();
    break;

  case 0x06: // LD B, d8
    registers.b = fetch8();
    break;

  case 0xC3: { // JP a16
    uint16_t address = fetch16();
    registers.pc = address;
    break;
  }

  case 0xAF: { // XOR A
    registers.a ^= registers.a;
    registers.f = 0x80; // Z=1, N=0, H=0, C=0
    break;
  }

  case 0x21: { // LD HL n16
    registers.hl = fetch16();
    break;
  }

  case 0x0E: { // LD C n8
    registers.c = fetch8();
    break;
  }

  case 0x32: { // LD (HL-) A
    memory.write(registers.hl, registers.a);
    registers.hl--;
    break;
  }

  case 0x05: { // DEC B
    uint8_t result = registers.b - 1;

    registers.f &= FLAG_C; // Preserve carry flag
    registers.f |= FLAG_N; // Set subtract flag

    if (result == 0) {
      registers.f |= FLAG_Z;
    }

    if ((registers.b & 0x0F) == 0) {
      registers.f |= FLAG_H;
    }

    registers.b = result;
    break;
  }

  case 0x0D: { // DEC C
    uint8_t result = registers.c - 1;

    registers.f &= FLAG_C; // Preserve carry flag
    registers.f |= FLAG_N; // Set subtract flag

    if (result == 0) {
      registers.f |= FLAG_Z;
    }

    if ((registers.c & 0x0F) == 0) {
      registers.f |= FLAG_H;
    }

    registers.c = result;
    break;
  }

  case 0xF3: { // DI
    ime = false;
    break;
  }

  case 0xFB: { // EI
    ime = true;
    break;
  }

  case 0x20: { // JR NZ, r8
    int8_t offset = static_cast<int8_t>(fetch8());
    if (!(registers.f & FLAG_Z)) {
      registers.pc += offset;
    }
    break;
  }

  case 0xE0: { // LDH (a8), A
    uint8_t address = fetch8();
    memory.write(0xFF00 + address, registers.a);
    break;
  }

  case 0xF0: { // LDH A, (a8)
    uint8_t offset = fetch8();
    registers.a = memory.read(0xFF00 + offset);
    break;
  }

  case 0xEA: { // LD (a16), A
    uint16_t address = fetch16();
    memory.write(address, registers.a);
    break;
  }

  case 0xFE: { // CP d8
    uint8_t value = fetch8();
    uint8_t result = registers.a - value;

    registers.f = FLAG_N; // CP always sets subtract

    if (registers.a == value)
      registers.f |= FLAG_Z;

    if ((registers.a & 0x0F) < (value & 0x0F))
      registers.f |= FLAG_H;

    if (registers.a < value)
      registers.f |= FLAG_C;

    break;
  }

  default:
    std::cerr << "Unknown instruction: 0x" << std::hex << (int)instruction
              << '\n';
    return false;
  }

  return true;
}