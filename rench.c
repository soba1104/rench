#include "rench.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    options opts;
    buffer buf;
    pthread_t producer_thread;
    pthread_t consumer_thread;
    producer_args pargs;
    consumer_args cargs;

    init_options(&opts);
    parse_options(&opts, argc, argv);
    validate_options(&opts);
    fprintf(stderr, "file = %s, bitrate = %u, bufsize = %u\n", opts.file, opts.bitrate, opts.bufsize);

    buffer_init(&buf, opts.bufsize);
    producer_init_args(&pargs, &buf, opts.file);
    consumer_init_args(&cargs, &buf, opts.bitrate);

    pthread_create(&producer_thread, NULL, producer_main, &pargs);
    pthread_create(&consumer_thread, NULL, consumer_main, &cargs);
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

    buffer_free(&buf);
    free_options(&opts);

    return 0;
}
