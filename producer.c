#include "rench.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h>
#include <assert.h>
#include <unistd.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

void *producer_main(void *ptr) {
    producer_args *args = ptr;
    char *localbuf = NULL;
    buffer *buf = args->buf;
    uint32_t bufsize = buffer_get_size(buf);
    uint32_t unit = args->unit;
    char *file = args->file;
    fops *fops = args->fops;
    bool debug = args->debug;

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
        bool producible;

        producible = buffer_wait_producible(buf);
        if (!producible) {
            goto out;
        }
        bufidx = buffer_get_idx(buf);
        toread = MIN(unit, bufsize - bufidx);
        assert(toread > 0);
        ret = fops_read(fops, localbuf, toread);
        if (ret > 0) {
            if (debug) {
                fprintf(stdout, "read complete %d bytes\n", ret);
            }
            buffer_produce(buf, ret);
        } else if (!ret) {
            if (debug) {
                fprintf(stdout, "produce end\n");
            }
            goto out;
        } else {
            perror("failed to read: ");
            goto out;
        }
    }

out:
    buffer_produce_end(buf);
    fops_close(fops);
    if (localbuf) {
        free(localbuf);
    }
    return NULL;
}

void producer_init_args(producer_args *args,
                        buffer *buf,
                        uint32_t unit,
                        char *file,
                        fops *fops,
                        bool debug) {
    args->buf = buf;
    args->unit = unit;
    args->file = file;
    args->fops = fops;
    args->debug = debug;
}
