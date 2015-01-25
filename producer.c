#include "rench.h"
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
    fops *fops = args->fops;

    if (!fops_open(fops)) {
        fprintf(stderr, "failed to open %s: ", file);
        perror(NULL);
        goto out;
    }

    localbuf = malloc(bufsize);
    if (!localbuf) {
        fprintf(stderr, "failed to allocate local buffer\n");
        goto out;
    }

    while (true) {
        uint32_t bufidx;
        int toread, ret;

        buffer_wait_producible(buf);
        bufidx = buffer_get_idx(buf);
        toread = bufsize - bufidx;
        assert(toread > 0);
        ret = fops_read(fops, localbuf, toread);
        if (ret > 0) {
            buffer_produce(buf, ret);
        } else if (!ret) {
            buffer_eof(buf);
            goto out;
        } else {
            perror("failed to read: ");
            goto out;
        }
    }

out:
    fops_close(fops);
    if (localbuf) {
        free(localbuf);
    }
    return NULL;
}

void producer_init_args(producer_args *args, buffer *buf, char *file, fops *fops) {
    args->buf = buf;
    args->file = file;
    args->fops = fops;
}
