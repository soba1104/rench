#include "rench.h"

int main(int argc, char *argv[]) {
    options opts;
    buffer buf;
    pthread_t producer_thread;
    pthread_t consumer_thread;
    producer_args pargs;
    consumer_args cargs;
    fops *fops;
    int ret = 0;

    init_options(&opts);
    parse_options(&opts, argc, argv);
    validate_options(&opts);

    switch (opts.type) {
        case POSIX:
            fops = fops_posix_new(opts.file);
            fprintf(stderr,
                    "type = posix, file = %s, bitrate = %u, bufsize = %u\n",
                    opts.file, opts.bitrate, opts.bufsize);
            break;
        case UNDEF:
            fprintf(stderr, "bug: should not be reached.\n");
            goto out;
    }
    if (!fops) {
        fprintf(stderr, "failed to allocate fops\n");
        ret = -1;
        goto out;
    }

    buffer_init(&buf, opts.bufsize);
    producer_init_args(&pargs, &buf, opts.file, fops);
    consumer_init_args(&cargs, &buf, opts.bitrate);

    pthread_create(&producer_thread, NULL, producer_main, &pargs);
    pthread_create(&consumer_thread, NULL, consumer_main, &cargs);
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

out:
    fops_close(fops);
    buffer_free(&buf);
    free_options(&opts);

    return ret;
}
