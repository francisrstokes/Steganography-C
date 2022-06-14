#ifndef STENO_H
#define STENO_H

#include "common.h"
#include "ppm.h"

bool stegno_rgb(uint8_t* imageData, const size_t imageDataSize, const uint8_t* data, const uint16_t dataSize);
uint8_t* stegno_rgb_read(uint8_t* imageData, const size_t imageDataSize, uint16_t* dataSize);

#endif // STENO_H
