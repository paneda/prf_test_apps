#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif
// matches stm32 default crc32 calculation with a polynom of: 0x04C11DB7
// https://stackoverflow.com/a/57940471/268541
uint32_t calcCrc32(const uint8_t* data, unsigned len);

#ifdef __cplusplus
}
#endif

