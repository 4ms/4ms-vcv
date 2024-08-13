#pragma once
#include <cstdint>
#include <span>

bool flash_read(std::span<uint8_t> data, uint32_t address);
bool flash_read(std::span<uint32_t> data, uint32_t address);
bool flash_erase_sector(uint32_t sector_start_address);
bool flash_write(std::span<const uint32_t> data, uint32_t address);
bool flash_erase_and_write(std::span<const uint8_t> data, uint32_t address);
