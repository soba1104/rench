#include "rench.h"
#include <assert.h>
#include <sys/time.h>

void buffer_init(buffer *buf, uint32_t size) {
    buf->size = size;
    buf->idx = 0;
    buf->eof = false;
    pthread_mutex_init(&buf->mutex, NULL);
    pthread_cond_init(&buf->producible_cond, NULL);
    pthread_cond_init(&buf->consumable_cond, NULL);
}

void buffer_free(buffer *buf) {
    // nothing to do
}

void buffer_eof(buffer *buf) {
    pthread_mutex_lock(&buf->mutex);
    buf->eof = true;
    pthread_cond_signal(&buf->consumable_cond);
    pthread_mutex_unlock(&buf->mutex);
}

void buffer_wait_producible(buffer *buf) {
    pthread_mutex_lock(&buf->mutex);
    assert(buf->idx <= buf->size);
    if (buf->idx == buf->size) { // buffer full
        pthread_cond_wait(&buf->producible_cond, &buf->mutex);
    }
    assert(buf->idx < buf->size);
    pthread_mutex_unlock(&buf->mutex);
}

void buffer_produce(buffer *buf, uint32_t size) {
    pthread_mutex_lock(&buf->mutex);
    buf->idx += size;
    assert(buf->idx <= buf->size);
    pthread_cond_signal(&buf->consumable_cond);
    pthread_mutex_unlock(&buf->mutex);
}

bool buffer_wait_consumable(buffer *buf, uint32_t size, uint64_t *time) {
    bool consumable;
    struct timeval s, e;

    pthread_mutex_lock(&buf->mutex);
    assert(buf->idx <= buf->size);
    gettimeofday(&s, NULL);
    e = s;
    while (true) {
        if (buf->idx >= size) {
            consumable = true;
            break;
        } else if (buf->eof) {
            consumable = false;
            break;
        } else {
            pthread_cond_wait(&buf->consumable_cond, &buf->mutex);
            gettimeofday(&e, NULL);
        }
    }
    *time = (e.tv_sec * 1000000 + e.tv_usec) - (s.tv_sec * 1000000 + s.tv_usec);
    pthread_mutex_unlock(&buf->mutex);

    return consumable;
}

void buffer_consume(buffer *buf, uint32_t size) {
    pthread_mutex_lock(&buf->mutex);
    assert(buf->idx >= size);
    buf->idx -= size;
    pthread_cond_signal(&buf->producible_cond);
    pthread_mutex_unlock(&buf->mutex);
}

uint32_t buffer_get_size(buffer *buf) {
    uint32_t bufsize;

    pthread_mutex_lock(&buf->mutex);
    bufsize = buf->size;
    pthread_mutex_unlock(&buf->mutex);

    return bufsize;
}

uint32_t buffer_get_idx(buffer *buf) {
    uint32_t bufidx;

    pthread_mutex_lock(&buf->mutex);
    bufidx = buf->idx;
    pthread_mutex_unlock(&buf->mutex);

    return bufidx;
}
