#include "stegno.h"

static inline uint8_t* stegno_writeByte(uint8_t* imagePtr, uint8_t byte) {
  for (int i = 0; i < 4; i++) {
    *imagePtr = (*imagePtr & 0xfc) | (byte & 0x03);
    byte >>= 2;
    imagePtr++;
  }
  return imagePtr;
}

static inline uint8_t stegno_readByte(uint8_t* imagePtr) {
  uint8_t byte = 0;

  for (int i = 0; i < 4; i++) {
    byte |= (*imagePtr & 0x03) << (i * 2);
    imagePtr++;
  }

  return byte;
}

bool stegno_rgb(uint8_t* imageData, const size_t imageDataSize, const uint8_t* data, const uint16_t dataSize) {
  // We will write 2 bits per byte, so we need 4 bytes to write a whole byte of secret data
  // We need an additional 8 bytes to write the size of the secret data as a LE uint16_t
  size_t requiredBytes = (imageDataSize * 4) + 8;

  if (dataSize > requiredBytes) {
    return false;
  }

  uint8_t* ptr = (uint8_t*)data;
  uint8_t* end = ptr + dataSize;

  // First write the size of the data
  imageData = stegno_writeByte(imageData, dataSize & 0xff);
  imageData = stegno_writeByte(imageData, (dataSize >> 8) & 0xff);

  // Write the message data into the image stream
  while (ptr != end) {
    imageData = stegno_writeByte(imageData, *ptr);
    ptr++;
  }

  return true;
}

uint8_t* stegno_rgb_read(uint8_t* imageData, const size_t imageDataSize, uint16_t* dataSize) {
  // Need at least 8 bytes in order to read the size
  if (imageDataSize < 8) {
    return NULL;
  }

  uint16_t dataBytes = stegno_readByte(imageData);
  imageData += 4;
  dataBytes |= stegno_readByte(imageData) << 8;
  imageData += 4;

  if (dataBytes == 0) {
    return NULL;
  }

  size_t requiredBytes = 8 + dataBytes * 4;
  if (imageDataSize < requiredBytes) {
    return NULL;
  }

  *dataSize = dataBytes;

  uint8_t* buf = STEGNO_MALLOC(dataBytes);
  uint8_t* ptr = buf;
  uint8_t* end = ptr + dataBytes;

  while (ptr != end) {
    *ptr = stegno_readByte(imageData);
    imageData += 4;
    ptr++;
  }

  return buf;
}
