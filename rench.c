#include "rench.h"

int main(int argc, char *argv[]) {
    options opts;
    task *tasks;
    glfs_t *glfs = NULL;
    int ret = 0, i, numfiles;

    if (argc == 1) {
        options_show_help();
        return 0;
    }
    options_init(&opts);
    options_parse(&opts, argc, argv);
    options_validate(&opts);

    numfiles = argc - optind;
    if (numfiles <= 0) {
        fprintf(stderr, "usage: rench [options] file1 file2 file3 ...\n");
        return -1;
    }
    if (opts.debug) {
        fprintf(stdout, "numfiles = %d, concurrency = %d\n", numfiles, numfiles);
    }

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
    tasks = malloc(sizeof(task) * numfiles);
    if (!tasks) {
        fprintf(stderr, "failed to allocate tasks\n");
        ret = -1;
        goto out;
    }
    memset(tasks, 0, sizeof(task) * numfiles);

    for (i = 0; i < numfiles; i++) {
        task *t = &tasks[i];
        if (!task_init(t, &opts, glfs)) {
            fprintf(stderr, "failed to init task\n");
        }
        task_run(t);
    }
    for (i = 0; i < numfiles; i++) {
        task_join(&tasks[i]);
    }

out:
    if (glfs) {
        glfs_fini(glfs);
    }
    if (tasks) {
        for (i = 0; i < numfiles; i++) {
            task_free(&tasks[i]);
        }
        free(tasks);
    }
    options_free(&opts);

    return ret;
}
