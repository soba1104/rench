#include <inttypes.h>

#define DEFAULT_BITRATE (128 * 1024)

typedef struct __options {
  uint32_t bitrate;
  char *file;
  char *host;
  int port;
} options;


