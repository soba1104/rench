#include "rench.h"

bool task_init(char *file, task *t, options *opts, glfs_t *glfs, rados_ioctx_t rados_ioctx) {
    t->fops = fops_new(file, opts, glfs, rados_ioctx);
    if (!t->fops) {
        return false;
    }
    buffer_init(&t->buf, opts->bufsize, opts->lower);
    producer_init_args(&t->pargs, &t->buf, opts->upper, file, t->fops, opts->debug);
    consumer_init_args(&t->cargs, &t->buf, opts->byterate, opts->count, opts->debug);
    return true;
}

void task_run(task *t) {
    pthread_create(&t->producer_thread, NULL, producer_main, &t->pargs);
    pthread_create(&t->consumer_thread, NULL, consumer_main, &t->cargs);
}

void task_join(task *t) {
    pthread_join(t->producer_thread, NULL);
    pthread_join(t->consumer_thread, NULL);
}

void task_free(task *t) {
    if (t->fops) {
        fops_free(t->fops);
    }
}
