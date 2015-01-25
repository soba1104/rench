#include <inttypes.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>

#define DEFAULT_BITRATE (128 * 1024)
#define DEFAULT_BUFSIZE (10 * 1024 * 1024)

typedef enum {
    UNDEF,
    POSIX
} fops_type;

typedef struct __options {
    uint32_t bitrate;
    uint32_t bufsize;
    char *file;
    char *host;
    int port;
    fops_type type;
} options;

typedef struct __buffer {
    uint32_t size;
    uint32_t idx;
    pthread_mutex_t mutex;
    pthread_cond_t consumable_cond;
    pthread_cond_t producible_cond;
    bool eof;
} buffer;

typedef struct __fops {
    void *state;
    bool (*open)(void*);
    int (*read)(void*, void*, int);
    void (*close)(void*);
} fops;

typedef struct __consumer_args {
    buffer *buf;
    uint32_t bitrate;
} consumer_args;

typedef struct __producer_args {
    buffer *buf;
    char *file;
    fops *fops;
} producer_args;

bool fops_open(fops *fops);
int fops_read(fops *fops, void *buf, int len);
void fops_close(fops *fops);
void fops_free(fops *fops);
fops *fops_posix_new(char *path);

void options_init(options *opts);
void options_parse(options *opts, int argc, char *argv[]);
void options_validate(options *opts);
void options_free(options *opts);

void *consumer_main(void *arg);
void consumer_init_args(consumer_args *args, buffer *buf, uint32_t bitrate);

void *producer_main(void *ptr);
void producer_init_args(producer_args *args, buffer *buf, char *file, fops *fops);

void buffer_init(buffer *buf, uint32_t size);
void buffer_free(buffer *buf);
void buffer_eof(buffer *buf);
void buffer_wait_producible(buffer *buf);
void buffer_produce(buffer *buf, uint32_t size);
bool buffer_wait_consumable(buffer *buf, uint32_t size, uint64_t *time);
void buffer_consume(buffer *buf, uint32_t size);
uint32_t buffer_get_size(buffer *buf);
uint32_t buffer_get_idx(buffer *buf);
