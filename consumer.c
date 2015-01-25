#include <rench.h>

#include <stdio.h>
#include <unistd.h>

void *consumer_main(void *ptr) {
	consumer_args *args = ptr;
	buffer *buf = args->buf;
	uint32_t bitrate = args->bitrate;
	int i;

	fprintf(stdout, "start consumer\n");
	for (i = 0; i < 10; i++) {
		bool available = buffer_consume(buf, bitrate);
		if (!available) {
			fprintf(stderr, "data unavailable.\n");
		}
		sleep(1);
	}
	return NULL;
}

void consumer_init_args(consumer_args *args, buffer *buf, uint32_t bitrate) {
	args->buf = buf;
	args->bitrate = bitrate;
}
