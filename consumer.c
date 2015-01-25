#include "rench.h"
#include <unistd.h>

void *consumer_main(void *ptr) {
    consumer_args *args = ptr;
    buffer *buf = args->buf;
    uint32_t bitrate = args->bitrate;
    int i;
    bool init = true, debug = args->debug;
    uint64_t consumed = 0;

    while (true) {
        uint64_t time;
        bool consumable = buffer_wait_consumable(buf, bitrate, &time);
        if (init) {
            fprintf(stdout, "initialize %lluusec\n", time);
            init = false;
        } else if (time > 0) {
            fprintf(stdout, "unavailable %lluusec\n", time);
        } else if (debug) {
            fprintf(stdout, "available\n");
        }
        if (!consumable) { // eof
            return NULL;
        }
        buffer_consume(buf, bitrate);
        consumed += bitrate;
        if (debug) {
            fprintf(stderr, "consumed %llubytes\n", consumed);
        }
        sleep(1);
    }
}

void consumer_init_args(consumer_args *args, buffer *buf, uint32_t bitrate, bool debug) {
    args->buf = buf;
    args->bitrate = bitrate;
    args->debug = debug;
}
