#include "rench.h"
#include <unistd.h>
#include <time.h>

void *consumer_main(void *ptr) {
    consumer_args *args = ptr;
    buffer *buf = args->buf;
    uint32_t byterate = args->byterate;
    uint32_t count = args->count;
    int i, c = 0;
    bool init = true, debug = args->debug;
    uint64_t consumed = 0;

    while (true) {
        uint64_t t;
        time_t s = time(NULL);
        bool consumable = buffer_wait_consumable(buf, byterate, &t);
        if (init) {
            fprintf(stdout, "%ld initialize %" PRIu64 "usec\n", s, t);
            init = false;
        } else if (t > 0) {
            fprintf(stdout, "%ld unavailable %" PRIu64 "usec\n", s, t);
        } else if (debug) {
            fprintf(stdout, "available\n");
        }
        if (!consumable) { // eof
            return NULL;
        }
        buffer_consume(buf, byterate);
        consumed += byterate;
        if (debug) {
            fprintf(stderr, "consumed %" PRIu64 "bytes\n", consumed);
        }
        c++;
        if (count > 0 && c >= count) {
            if (debug) {
                fprintf(stderr, "consume end\n");
            }
            buffer_consume_end(buf);
            return NULL;
        }
        sleep(1);
    }
}

void consumer_init_args(consumer_args *args, buffer *buf, uint32_t byterate, uint32_t count, bool debug) {
    args->buf = buf;
    args->byterate = byterate;
    args->count = count;
    args->debug = debug;
}
