#include <rench.h>
#include <assert.h>

void buffer_init(buffer *buf, uint32_t size) {
	buf->size = size;
	buf->idx = 0;
	pthread_mutex_init(&buf->mutex, NULL);
}

void buffer_free(buffer *buf) {
	// nothing to do
}

void buffer_produce(buffer *buf, uint32_t size) {
	pthread_mutex_lock(&buf->mutex);
	buf->idx += size;
	assert(buf->idx <= buf->size);
	pthread_mutex_unlock(&buf->mutex);
}

bool buffer_consume(buffer *buf, uint32_t size) {
	bool available;

	pthread_mutex_lock(&buf->mutex);
	if (buf->idx >= size) {
		buf->idx -= size;
		available = true;
	} else {
		available = false;
	}
	pthread_mutex_unlock(&buf->mutex);

	return available;
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
