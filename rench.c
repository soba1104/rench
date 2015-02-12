#include "rench.h"

int main(int argc, char *argv[]) {
    options opts;
    fops *fops;
    int ret = 0, i;

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
                        "type = posix, file = %s, byterate = %u, upper = %u, lower = %u, bufsize = %u, concurrency = %u\n",
                        opts.file, opts.upper, opts.lower, opts.byterate, opts.bufsize, opts.concurrency);
            }
            break;
        case GFAPI:
            fops = fops_gfapi_new(opts.host, opts.port, opts.volume, opts.file);
            if (opts.debug) {
                fprintf(stdout,
                        "type = gfapi, host = %s, port = %d, volume = %s, file = %s, byterate = %u, upper = %u, lower = %u, bufsize = %u, concurrency = %u\n",
                        opts.host, opts.port, opts.volume, opts.file, opts.upper, opts.lower, opts.byterate, opts.bufsize, opts.concurrency);
            }
            break;
    }
    if (!fops) {
        fprintf(stderr, "failed to allocate fops\n");
        ret = -1;
        goto out;
    }

    for (i = 0; i < opts.concurrency; i++) {
        buffer buf;
        producer_args pargs;
        consumer_args cargs;
        pthread_t producer_thread;
        pthread_t consumer_thread;

        buffer_init(&buf, opts.bufsize, opts.lower);
        producer_init_args(&pargs, &buf, opts.upper, opts.file, fops, opts.debug);
        consumer_init_args(&cargs, &buf, opts.byterate, opts.count, opts.debug);

        pthread_create(&producer_thread, NULL, producer_main, &pargs);
        pthread_create(&consumer_thread, NULL, consumer_main, &cargs);
        pthread_join(producer_thread, NULL);
        pthread_join(consumer_thread, NULL);

        buffer_free(&buf);
    }

out:
    fops_free(fops);
    options_free(&opts);

    return ret;
}
