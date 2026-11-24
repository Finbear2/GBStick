#include "memory.h"
#include <cstdint>
#include <fstream>
#include <iostream>

bool Memory::loadRom(const std::string &path) {

  std::ifstream file(path, std::ios::binary | std::ios::ate);

  if (!file.is_open()) {
    std::cerr << "Failed to open ROM: " << path << '\n';
    return false;
  }

  // Find the file size.
  std::streamsize size = file.tellg();

  if (size <= 0) {
    std::cerr << "ROM is empty or invalid.\n";
    return false;
  }

  // Allocate enough space for the ROM.
  rom.resize(static_cast<size_t>(size));

  // Go back to the beginning.
  file.seekg(0, std::ios::beg);

  // Read the binary data.
  if (!file.read(reinterpret_cast<char *>(rom.data()), size)) {
    std::cerr << "Failed to read ROM.\n";
    return false;
  }

  std::cout << "Loaded ROM: " << path << '\n';
  std::cout << "ROM size: " << rom.size() << " bytes\n";

  return true;
}

uint8_t Memory::read(uint16_t address) {

  // Cartridge ROM area.
  if (address <= 0x7FFF) {
    if (address < rom.size()) {
      return rom[address];
    }

    return 0xFF;
  }

  if (address == 0xFF44) {
    return ly;
  }

  return data[address];
}

void Memory::write(uint16_t address, uint8_t value) {

  // Cartridge ROM area.
  if (address <= 0x7FFF) {
    if (address < rom.size()) {
      return;
    }
  }

  data[address] = value;
}

void Memory::updateGraphics() {
  ly++;

  if (ly > 153) {
    ly = 0;
  }
}