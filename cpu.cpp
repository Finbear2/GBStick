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

  cycles++;
  targetCycles--;

  if (targetCycles > 0) {
    return true;
  }

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

  switch (instruction) {

  case 0x00: // NOP
    targetCycles = 4;
    break;

  case 0x01: // LD BC n16
    targetCycles = 12;
    registers.bc = fetch16();
    break;

  case 0x02: // LD BC A
    targetCycles = 8;
    memory.write(registers.bc, registers.a);
    break;

  case 0x03: // INC BC
    targetCycles = 8;
    registers.bc++;
    break;

  case 0x04: // INC B
    targetCycles = 4;
    uint8_t result = registers.b + 1;

    registers.f &= FLAG_C; // Preserve carry
    if (result == 0)
      registers.f |= FLAG_Z;
    if ((registers.b & 0x0F) == 0x0F)
      registers.f |= FLAG_H;

    registers.b = result;
    break;

  case 0x05: // DEC B
    targetCycles = 4;
    result = registers.b - 1;

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
    
  case 0x36: { // LD (HL), d8
    uint8_t value = fetch8();
    memory.write(registers.hl, value);
    break;
  }

  case 0x22: { // LD (HL+), A
    memory.write(registers.hl, registers.a);
    registers.hl++;
    break;
  }

  case 0x77: { // LD (HL), A
    memory.write(registers.hl, registers.a);
    break;
  }

  case 0x31: { // LD SP, d16
    registers.sp = fetch16();
    break;
  }

  case 0x2A: { // LD A, (HL+)
    registers.a = memory.read(registers.hl);
    registers.hl++;
    break;
  }

  case 0xE2: { // LDH (C), A
    memory.write(0xFF00 + registers.c, registers.a);
    break;
  }

  case 0x0C: { // INC C
    uint8_t result = registers.c + 1;

    registers.f &= FLAG_C; // Preserve carry
    if (result == 0)
      registers.f |= FLAG_Z;
    if ((registers.c & 0x0F) == 0x0F)
      registers.f |= FLAG_H;

    registers.c = result;
    break;
  }

  case 0xCD: { // CALL a16
    uint16_t address = fetch16();

    registers.sp--;
    memory.write(registers.sp, (registers.pc >> 8) & 0xFF);

    registers.sp--;
    memory.write(registers.sp, registers.pc & 0xFF);

    registers.pc = address;
    break;
  }

  case 0x0B: { // DEC BC
    registers.bc--;
    break;
  }

  case 0x78: { // LD A B
    registers.a = registers.b;
    break;
  }

  case 0xB1: { // OR C
    registers.a |= registers.c;

    registers.f = 0; // Clear all flags
    if (registers.a == 0)
      registers.f |= FLAG_Z;

    break;
  }

  case 0xC9: { // RET
    uint8_t low = memory.read(registers.sp);
    registers.sp++;

    uint8_t high = memory.read(registers.sp);
    registers.sp++;

    registers.pc = (high << 8) | low;
    break;
  }

  case 0x11: { // LD DE (n16)
    registers.de = fetch16();
    break;
  }

  case 0x12: { // LD (DE) A
    memory.write(registers.de, registers.a);
    break;
  }

  case 0x13: { // INC DE
    registers.de++;
    break;
  }

  case 0xFA: { // LD A, (a16)
    uint16_t address = fetch16();
    registers.a = memory.read(address);
    break;
  }

  case 0xA7: { // AND A
    registers.a &= registers.a;

    registers.f = FLAG_H; // H is always set
    if (registers.a == 0)
      registers.f |= FLAG_Z;

    break;
  }

  case 0x28: { // JR Z, r8
    int8_t offset = static_cast<int8_t>(fetch8());

    if (registers.f & FLAG_Z) {
      registers.pc += offset;
    }

    break;
  }

  case 0xC0: { // RET NZ
    if (!(registers.f & FLAG_Z)) {
      uint8_t low = memory.read(registers.sp++);
      uint8_t high = memory.read(registers.sp++);

      registers.pc = (high << 8) | low;
    }

    break;
  }

  case 0x2F: { // CPL
    registers.a = ~registers.a;

    registers.f |= FLAG_N | FLAG_H; // Set N and H, preserve Z and C
    break;
  }

  

  default:
    std::cerr << "Unknown instruction: 0x" << std::hex << (int)instruction
              << '\n';
    return false;
  }

  memory.updateGraphics();

  return true;
}