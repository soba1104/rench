#include <rench.h>
#include <assert.h>
#include <sys/time.h>
#include <stdio.h>

void buffer_init(buffer *buf, uint32_t size) {
    buf->size = size;
    buf->idx = 0;
    buf->eof = false;
    pthread_mutex_init(&buf->mutex, NULL);
    pthread_cond_init(&buf->cond, NULL);
}

void buffer_free(buffer *buf) {
    // nothing to do
}

void buffer_eof(buffer *buf) {
    pthread_mutex_lock(&buf->mutex);
    buf->eof = true;
    pthread_cond_signal(&buf->cond);
    pthread_mutex_unlock(&buf->mutex);
}

void buffer_produce(buffer *buf, uint32_t size) {
    pthread_mutex_lock(&buf->mutex);
    buf->idx += size;
    assert(buf->idx <= buf->size);
    pthread_cond_signal(&buf->cond);
    pthread_mutex_unlock(&buf->mutex);
}

uint64_t buffer_consume(buffer *buf, uint32_t size, uint64_t *time) {
    bool eof;

    pthread_mutex_lock(&buf->mutex);
    eof = buf->eof;
    if (eof) {
        *time = 0;
    } else if (buf->idx >= size) {
        buf->idx -= size;
        if (time) {
            *time = 0;
        }
    } else {
        struct timeval s, e;
        gettimeofday(&s, NULL);
        pthread_cond_wait(&buf->cond, &buf->mutex);
        gettimeofday(&e, NULL);
        eof = buf->eof;
        if (time) {
            *time = (e.tv_sec * 1000000 + e.tv_usec) - (s.tv_sec * 1000000 + s.tv_usec);
        }
        if (!eof) {
            assert(buf->idx >= size);
            buf->idx -= size;
        }
    }
    pthread_mutex_unlock(&buf->mutex);

    return eof;
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
