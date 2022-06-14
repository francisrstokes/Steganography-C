#ifndef PPM_H
#define PPM_H

#include "common.h"

typedef struct ppm_image_t {
  uint16_t width;
  uint16_t height;
  uint8_t* data;
} ppm_image_t;

enum ppm_read_state {
  PPM_VERSION,
  PPM_WIDTH,
  PPM_HEIGHT,
  PPM_MAX_COLOR,
};

ppm_image_t* ppm_create(const uint16_t width, const uint16_t height);
void ppm_destroy(ppm_image_t* image);
bool ppm_write(const ppm_image_t* image, const char* path);
ppm_image_t* ppm_read(const char* path);

#endif // PPM_H

