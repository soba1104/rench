#include <inttypes.h>
#include <pthread.h>
#include <stdbool.h>

#define DEFAULT_BITRATE (128 * 1024)
#define DEFAULT_BUFSIZE (10 * 1024 * 1024)

typedef struct __options {
  uint32_t bitrate;
  uint32_t bufsize;
  char *file;
} options;

typedef struct __buffer {
	uint32_t size;
	uint32_t idx;
	pthread_mutex_t mutex;
	pthread_cond_t cond;
	bool eof;
} buffer;

typedef struct __consumer_args {
	buffer *buf;
	uint32_t bitrate;
} consumer_args;

typedef struct __producer_args {
	buffer *buf;
	char *file;
} producer_args;

void illegal_option(char *errmsg);
void init_options(options *opts);
void set_bitrate_option(options *opts, char *bitrate);
void set_bufsize_option(options *opts, char *bufsize);
void set_file_option(options *opts, char *file);
void parse_options(options *opts, int argc, char *argv[]);
void validate_options(options *opts);
void free_options(options *opts);

void *consumer_main(void *arg);
void consumer_init_args(consumer_args *args, buffer *buf, uint32_t bitrate);

void *producer_main(void *ptr);
void producer_init_args(producer_args *args, buffer *buf, char *file);

void buffer_init(buffer *buf, uint32_t size);
void buffer_free(buffer *buf);
void buffer_eof(buffer *buf);
void buffer_produce(buffer *buf, uint32_t size);
uint64_t buffer_consume(buffer *buf, uint32_t size, uint64_t *time);
uint32_t buffer_get_size(buffer *buf);
uint32_t buffer_get_idx(buffer *buf);
