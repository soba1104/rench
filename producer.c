#include <rench.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>

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


void *producer_main(void *ptr) {
	producer_args *args = ptr;
  char *localbuf = NULL;
	buffer *buf = args->buf;
	uint32_t bufsize = buffer_get_size(buf);
  char *file = args->file;
  int fd = -1;

  fd = open(file, O_RDONLY);
  if (fd < 0) {
    fprintf(stderr, "failed to open %s: ", file);
    perror(NULL);
    goto out;
  }

  localbuf = malloc(bufsize);
  if (!localbuf) {
    fprintf(stderr, "failed to allocate local buffer\n");
    goto out;
  }

  while (1) {
		uint32_t bufidx = buffer_get_idx(buf);
    int toread = bufsize - bufidx;
    assert(toread >= 0);

    if (toread > 0) { // buffer remains toread bytes.
      int ret = read(fd, localbuf, toread);
      if (!ret) { // EOF
				goto out;
      } else if (ret < 0) {
        perror("failed to read: ");
      }
			buffer_produce(buf, ret);
    } else { // toread == 0, buffer full.
			fprintf(stderr, "Zzz...\n");
      sleep(1); // TODO use pthread_cond_wait
    }
  }

out:
  if (fd >= 0) {
    close(fd);
  }
	if (localbuf) {
		free(localbuf);
	}
	return NULL;
}

void producer_init_args(producer_args *args, buffer *buf, char *file) {
	args->buf = buf;
	args->file = file;
}
