#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>

#include "rench.h"

void loop(int fd, char *buf, uint32_t bufsize, uint32_t bitrate) {
  int bufidx = 0;
  int progress = 0;

  while (1) {
    struct timeval starttime, endtime;
    int toread = bufsize - bufidx;
    uint64_t usec;
    assert(toread >= 0);

    gettimeofday(&starttime, NULL);
    if (toread > 0) { // buffer remains toread bytes.
      int ret = read(fd, buf+bufidx, toread);
      if (!ret) { // EOF
        return;
      } else if (ret < 0) {
        perror("failed to read: ");
      }
      bufidx += ret;
    } else { // toread == 0, buffer full.
      bufidx -= bitrate;
      sleep(1);
    }
    gettimeofday(&endtime, NULL);

    usec = (endtime.tv_sec * 1000000 + endtime.tv_usec) - (starttime.tv_sec * 1000000 + starttime.tv_usec);
    fprintf(stderr, "usec = %lld\n", usec);
  }
}

void run(options *opts) {
  char *file = opts->file;
  char *buf;
  uint32_t bufsize = opts->bufsize;
  uint32_t bitrate = opts->bitrate;
  int fd;

  buf = malloc(bufsize);
  if (!buf) {
    fprintf(stderr, "failed to allocate buffer\n");
    goto out;
  }

  fd = open(file, O_RDONLY);
  if (fd < 0) {
    fprintf(stderr, "failed to open %s: ", file);
    perror(NULL);
    goto out;
  }

  loop(fd, buf, bufsize, bitrate);

out:
  if (buf) {
    free(buf);
  }
  if (fd >= 0) {
    close(fd);
  }
}

int main(int argc, char *argv[]) {
  options opts;

  init_options(&opts);
  parse_options(&opts, argc, argv);
  validate_options(&opts);

  fprintf(stderr, "file = %s, bitrate = %u, bufsize = %u\n", opts.file, opts.bitrate, opts.bufsize);
  run(&opts);

  free_options(&opts);

  return 0;
}
