#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>

#ifndef STEGNO_MALLOC
#include <malloc.h>
#define STEGNO_MALLOC  malloc
#define STEGNO_FREE    free
#endif

#endif // COMMON_H

