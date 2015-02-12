#include "rench.h"

void task_init(task *t, options *opts, fops *fops) {
    buffer_init(&t->buf, opts->bufsize, opts->lower);
    producer_init_args(&t->pargs, &t->buf, opts->upper, opts->file, fops, opts->debug);
    consumer_init_args(&t->cargs, &t->buf, opts->byterate, opts->count, opts->debug);
}

void task_run(task *t) {
    pthread_create(&t->producer_thread, NULL, producer_main, &t->pargs);
    pthread_create(&t->consumer_thread, NULL, consumer_main, &t->cargs);
}

void task_join(task *t) {
    pthread_join(t->producer_thread, NULL);
    pthread_join(t->consumer_thread, NULL);
}
