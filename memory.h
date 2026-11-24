#pragma once

#include <cstdint>
#include <string>
#include <vector>

class Memory {
public:
  uint8_t data[0x10000] = {};

  std::vector<uint8_t> rom;

  bool loadRom(const std::string &path);

  uint8_t read(uint16_t address);

  void write(uint16_t address, uint8_t value);

  uint8_t ly = 0;
};