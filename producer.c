#include <rench.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>

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
            if (ret > 0) {
                buffer_produce(buf, ret);
            } else if (!ret) {
                buffer_eof(buf);
                goto out;
            } else {
                perror("failed to read: ");
            }
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
