#pragma once

#include <cstddef>
#include <cstdint>

#include "cse7761_core_registers.h"

namespace cse7761_core {

inline uint8_t checksum(const uint8_t *data, size_t size) {
  uint8_t sum = 0;
  for (size_t i = 0; i < size; i++) {
    sum += data[i];
  }
  return static_cast<uint8_t>(~sum);
}

inline size_t build_write_frame(uint8_t reg, uint16_t value, uint8_t out[5]) {
  out[0] = kFrameStart;
  out[1] = reg;
  size_t len = 2;

  if (value != 0) {
    if (value < 0xFF) {
      out[2] = static_cast<uint8_t>(value & 0xFF);
      len = 3;
    } else {
      out[2] = static_cast<uint8_t>((value >> 8) & 0xFF);
      out[3] = static_cast<uint8_t>(value & 0xFF);
      len = 4;
    }
    out[len] = checksum(out, len);
    len++;
  }
  return len;
}

inline bool validate_read_frame(uint8_t reg, const uint8_t *data, size_t size,
                                uint8_t received_crc) {
  uint8_t sum = kFrameStart + reg;
  for (size_t i = 0; i < size; i++) {
    sum += data[i];
  }
  return static_cast<uint8_t>(~sum) == received_crc;
}

inline uint32_t decode_be(const uint8_t *data, size_t size) {
  uint32_t result = 0;
  for (size_t i = 0; i < size; i++) {
    result = (result << 8) | data[i];
  }
  return result;
}

inline int32_t sign_extend_24(uint32_t value) {
  if ((value & 0x800000U) != 0) {
    return static_cast<int32_t>(value | 0xFF000000U);
  }
  return static_cast<int32_t>(value);
}

}  // namespace cse7761_core
