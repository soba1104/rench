#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "rench.h"

int main(int argc, char *argv[]) {
  options opts;

  init_options(&opts);
  parse_options(&opts, argc, argv);
  validate_options(&opts);

  fprintf(stderr, "target = %s:%u:%s, bitrate = %u\n", opts.host, (uint16_t)opts.port, opts.file, opts.bitrate);

  return 0;
}
