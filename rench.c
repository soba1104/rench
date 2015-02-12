#include "rench.h"

int main(int argc, char *argv[]) {
    options opts;
    fops *fops;
    task *tasks;
    glfs_t *glfs = NULL;
    int ret = 0, i;

    if (argc == 1) {
        options_show_help();
        return 0;
    }
    options_init(&opts);
    options_parse(&opts, argc, argv);
    options_validate(&opts);

    if (opts.type == GFAPI) {
        glfs = glfs_new(opts.volume);
        if (!glfs) {
            goto out;
        }
        glfs_set_volfile_server(glfs, "tcp", opts.host, opts.port);
        if (glfs_init(glfs) != 0) {
            goto out;
        }
    }
    fops = fops_new(&opts, glfs);
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
    if (glfs) {
        glfs_fini(glfs);
    }
    if (tasks) {
        free(tasks);
    }
    fops_free(fops);
    options_free(&opts);

    return ret;
}
