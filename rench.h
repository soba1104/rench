#include <inttypes.h>

#define DEFAULT_BITRATE (128 * 1024)
#define DEFAULT_BUFSIZE (10 * 1024 * 1024)

typedef struct __options {
  uint32_t bitrate;
  uint32_t bufsize;
  char *file;
} options;

void illegal_option(char *errmsg);
void init_options(options *opts);
void set_bitrate_option(options *opts, char *bitrate);
void set_bufsize_option(options *opts, char *bufsize);
void set_file_option(options *opts, char *file);
void parse_options(options *opts, int argc, char *argv[]);
void validate_options(options *opts);
void free_options(options *opts);
