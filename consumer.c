#include "rench.h"
#include <unistd.h>

void *consumer_main(void *ptr) {
    consumer_args *args = ptr;
    buffer *buf = args->buf;
    uint32_t bitrate = args->bitrate;
    int i;
    bool init = true;

    fprintf(stdout, "start consumer\n");
    while (true) {
        uint64_t time;
        bool consumable = buffer_wait_consumable(buf, bitrate, &time);
        if (init) {
            fprintf(stderr, "initialize %lluusec.\n", time);
            init = false;
        } else if (time > 0) {
            fprintf(stderr, "unavailable %lluusec.\n", time);
        }
        if (!consumable) { // eof
            fprintf(stdout, "end consumer\n");
            return NULL;
        }
        buffer_consume(buf, bitrate);
        sleep(1);
    }
}

void consumer_init_args(consumer_args *args, buffer *buf, uint32_t bitrate) {
    args->buf = buf;
    args->bitrate = bitrate;
}
