#include "util.h"
#include "ppm.h"
#include "stegno.h"
#include <string.h>

static void writeSteno() {
  ppm_image_t* image = ppm_read("Tux.ppm");
  if (image == NULL) {
    return;
  }
  size_t imageDataSize = image->width * image->height * 3;

  FILE* f = fopen("hideme.txt", "rb");
  size_t fileSize = GetFileSize(f);

  printf("Hiding %d bytes in image...\n", fileSize);

  uint8_t* fileData = STEGNO_MALLOC(fileSize);
  fread(fileData, fileSize, 1, f);

  stegno_rgb(image->data, imageDataSize, fileData, fileSize);
  STEGNO_FREE(fileData);

  ppm_write(image, "copy.ppm");
  ppm_destroy(image);
}

static void readSteno() {
  ppm_image_t* image = ppm_read("copy.ppm");
  if (image == NULL) {
    return;
  }
  size_t imageDataSize = image->width * image->height * 3;

  printf("Reading steganographic data from image...\n");

  uint16_t messageLen;
  uint8_t* message = stegno_rgb_read(image->data, imageDataSize, &messageLen);

  if (!message) {
    printf("No valid message!\n");
    return;
  }

  printf("Apparent bytes in message: %d\n", messageLen);

  FILE* f = fopen("out.txt", "wb");
  fwrite(message, messageLen, 1, f);
  fclose(f);

  STEGNO_FREE(message);
  ppm_destroy(image);
}

int main() {
  writeSteno();
  readSteno();

  return 0;
}
