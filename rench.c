#include "rench.h"

int main(int argc, char *argv[]) {
    options opts;
    task *tasks = NULL;
    rados_t rados_cluster = NULL;
    rados_ioctx_t rados_ioctx = NULL;
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
    if (opts.type == RADOS) {
        int err;

        err = rados_create2(&rados_cluster, opts.rados_cluster_name, opts.rados_user_name, 0);
        if (err < 0) {
            fprintf(stderr, "Failed to create rados cluster handle. Error = %s.\n", strerror(-err));
            goto out;
        }

        err = rados_conf_read_file(rados_cluster, opts.rados_config_path);
        if (err < 0) {
            fprintf(stderr, "Failed to read rados config file. Error = %s.\n", strerror(-err));
            goto out;
        }

        err = rados_connect(rados_cluster);
        if (err < 0) {
            fprintf(stderr, "Failed to connect to rados cluster. Error = %s.\n", strerror(-err));
            goto out;
        }

        err = rados_ioctx_create(rados_cluster, opts.rados_pool_name, &rados_ioctx);
        if (err < 0) {
            fprintf(stderr, "Failed to create rados ioctx. Error = %s.\n", strerror(-err));
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
        char *f = argv[optind + i];
        if (!task_init(f, t, &opts, glfs, rados_ioctx)) {
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
    if (rados_cluster) {
        rados_shutdown(rados_cluster);
    }
    if (rados_ioctx) {
        rados_ioctx_destroy(rados_ioctx);
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
