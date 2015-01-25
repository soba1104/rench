#include <rench.h>

#include <stdio.h>
#include <unistd.h>

void *consumer_main(void *ptr) {
	consumer_args *args = ptr;
	buffer *buf = args->buf;
	uint32_t bitrate = args->bitrate;
	int i;

	fprintf(stdout, "start consumer\n");
	while (true) {
		uint64_t time;
		bool eof = buffer_consume(buf, bitrate, &time);
		if (time > 0) {
			fprintf(stderr, "data unavailable: %llu usec.\n", time);
		}
		if (eof) {
			return NULL;
		}
		sleep(1);
	}
}

void consumer_init_args(consumer_args *args, buffer *buf, uint32_t bitrate) {
	args->buf = buf;
	args->bitrate = bitrate;
}
