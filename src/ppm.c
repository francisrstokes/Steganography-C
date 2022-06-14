#include <string.h>
#include "ppm.h"
#include "util.h"

#define MIN(a, b) (a < b ? a : b)

ppm_image_t* ppm_create(const uint16_t width, const uint16_t height) {
  ppm_image_t* image = STEGNO_MALLOC(sizeof(ppm_image_t));
  uint8_t* buf = STEGNO_MALLOC(width * height * 3);

  image->width = width;
  image->height = height;
  image->data = buf;

  return image;
}

void ppm_destroy(ppm_image_t* image) {
  STEGNO_FREE(image->data);
  STEGNO_FREE(image);
}

bool ppm_write(const ppm_image_t* image, const char* path) {
  FILE* f = fopen(path, "wb");

  if (f == NULL) {
    printf("Error: Couldn't open file \"%s\" for writing\n", path);
    return false;
  }

  char header[20] = {0};
  sprintf(header, "P6\n%d %d 255\n", image->width, image->height);
  size_t headerLen = strlen(header);

  fwrite(header, headerLen, 1, f);
  fwrite(image->data, image->width * image->height * 3, 1, f);

  fclose(f);

  return true;
}

static inline bool is_whitespace(const char c) {
  return c == ' ' || c == '\t' || c == '\r' || c == '\n';
}

static inline bool is_digit(const char c) {
  return c >= '0' && c <= '9';
}

static bool consume_whitespace(uint8_t* fileBuf, size_t fileSize, size_t* i) {
  size_t start = *i;
  while (*i < fileSize) {
    if (is_whitespace(fileBuf[*i])) {
      (*i)++;
    } else {
      return start != *i;
    }
  }
}

static bool consume_comment(uint8_t* fileBuf, size_t fileSize, size_t* i) {
  if (fileBuf[*i] == '#') {
    (*i)++;
    while (*i < fileSize) {
      if (fileBuf[*i] == '\n' || fileBuf[*i] == '\r') {
        (*i)++;
        return true;
      } else {
        (*i)++;
      }
    }
  }
  return false;
}

static bool read_uint16(uint16_t* n, uint8_t* fileBuf, size_t fileSize, size_t* i) {
  char uintStr[6] = {0};
  size_t idx = 0;
  bool valid = false;

  while (*i < fileSize) {
    char n = fileBuf[(*i)++];

    if (is_digit(n)) {
      uintStr[idx++] = n;
    } else if (is_whitespace(n)) {
      valid = idx > 0;
      break;
    } else {
      // Reading anything else is malformed
      return false;
    }

    // Number too long
    if (idx == 5) {
      return false;
    }
  }

  // Didn't read any number
  if (!valid) {
    return false;
  }

  *n = atoi(uintStr);
  return true;
}

ppm_image_t* ppm_read(const char* path) {
  FILE* f = fopen(path, "rb");

  if (f == NULL) {
    printf("Error: Couldn't open file \"%s\" for reading\n", path);
    return false;
  }

  size_t fileSize = GetFileSize(f);
  uint8_t* fileBuf = STEGNO_MALLOC(fileSize);
  fread(fileBuf, fileSize, 1, f);
  fclose(f);

  bool done = false;
  uint8_t readState = PPM_VERSION;
  size_t i = 0;

  uint16_t width;
  uint16_t height;
  uint16_t maxColorValue;
  uint16_t headerSize;

  // Parse & sanity check the file
  while (!done && (i < fileSize)) {
    if (consume_comment(fileBuf, fileSize, &i)) {
      continue;
    }

    if (consume_whitespace(fileBuf, fileSize, &i)) {
      continue;
    }

    switch (readState) {
      case PPM_VERSION: {
        if (fileBuf[i] == 'P' && fileBuf[i+1] == '6') {
          i += 2;
          readState = PPM_WIDTH;
          continue;
        }

        printf("Expected version number when reading ppm file\n");
        return NULL;
      }

      case PPM_WIDTH: {
        if (!read_uint16(&width, fileBuf, fileSize, &i)) {
          printf("Expected valid width when reading ppm file\n");
          return NULL;
        }

        readState = PPM_HEIGHT;
        continue;
      }

      case PPM_HEIGHT: {
        if (!read_uint16(&height, fileBuf, fileSize, &i)) {
          printf("Expected valid height when reading ppm file\n");
          return NULL;
        }

        readState = PPM_MAX_COLOR;
        continue;
      }

      case PPM_MAX_COLOR: {
        if (!read_uint16(&maxColorValue, fileBuf, fileSize, &i)) {
          printf("Expected valid max color value when reading ppm file\n");
          return NULL;
        }

        if (maxColorValue != 255) {
          printf("Invalid max color value %d in ppm header (only 255 is supported)\n", maxColorValue);
          return NULL;
        }

        headerSize = i;
        done = true;
        continue;
      }
    }
  }

  size_t expectedSize = headerSize + (width * height * 3);
  if (fileSize != expectedSize) {
    printf("PPM file reported %d bytes, but is actually %d bytes\n", expectedSize, fileSize);
    return NULL;
  }

  ppm_image_t* image = ppm_create(width, height);
  memcpy(image->data, &fileBuf[i], width * height * 3);
  STEGNO_FREE(fileBuf);

  return image;
}
