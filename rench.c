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

    if (argc == 1) {
        options_show_help();
        return 0;
    }
    options_init(&opts);
    options_parse(&opts, argc, argv);
    options_validate(&opts);

    switch (opts.type) {
        case POSIX:
            fops = fops_posix_new(opts.file);
            if (opts.debug) {
                fprintf(stdout,
                        "type = posix, file = %s, bitrate = %u, bufsize = %u\n",
                        opts.file, opts.bitrate, opts.bufsize);
            }
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
    producer_init_args(&pargs, &buf, opts.file, fops, opts.debug);
    consumer_init_args(&cargs, &buf, opts.bitrate, opts.debug);

    pthread_create(&producer_thread, NULL, producer_main, &pargs);
    pthread_create(&consumer_thread, NULL, consumer_main, &cargs);
    pthread_join(producer_thread, NULL);
    pthread_join(consumer_thread, NULL);

out:
    fops_free(fops);
    buffer_free(&buf);
    options_free(&opts);

    return ret;
}
