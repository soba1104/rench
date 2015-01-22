#include <inttypes.h>

#define DEFAULT_BITRATE (128 * 1024)
#define DEFAULT_BUFSIZE (10 * 1024 * 1024)

typedef struct __options {
  uint32_t bitrate;
  uint32_t bufsize;
  char *file;
} options;


