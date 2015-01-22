#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "rench.h"

void run(char *file) {
  int fd = open(file, O_RDONLY);
  if (fd < 0) {
    fprintf(stderr, "failed to open %s: ", file);
    perror(NULL);
    goto out;
  }

out:
  if (fd >= 0) {
    close(fd);
  }
}

int main(int argc, char *argv[]) {
  options opts;

  init_options(&opts);
  parse_options(&opts, argc, argv);
  validate_options(&opts);

  fprintf(stderr, "file = %s, bitrate = %u\n", opts.file, opts.bitrate);
  run(opts.file);

  free_options(&opts);

  return 0;
}
