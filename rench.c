#include "rench.h"

int main(int argc, char *argv[]) {
    options opts;
    fops *fops;
    task *tasks;
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
    tasks = malloc(sizeof(task) * opts.concurrency);
    if (!tasks) {
        fprintf(stderr, "failed to allocate tasks\n");
        ret = -1;
        goto out;
    }
    memset(tasks, 0, sizeof(task) * opts.concurrency);

    for (i = 0; i < opts.concurrency; i++) {
        task *t = &tasks[i];
        task_init(t, &opts, fops);
        task_run(t);
    }
    for (i = 0; i < opts.concurrency; i++) {
        task *t = &tasks[i];
        task_join(t);
    }

out:
    if (tasks) {
        free(tasks);
    }
    fops_free(fops);
    options_free(&opts);

    return ret;
}
