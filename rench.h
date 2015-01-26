#include <inttypes.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>

#define DEFAULT_BYTERATE (128 * 1024)
#define DEFAULT_BUFSIZE (10 * 1024 * 1024)

typedef enum {
    POSIX,
    GFAPI
} fops_type;

typedef struct __options {
    uint32_t byterate;
    uint32_t bufsize;
    uint32_t unit;
    uint32_t count;
    char *file;
    char *host;
    char *volume;
    int port;
    fops_type type;
    bool debug;
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
    void (*free)(void*);
} fops;

typedef struct __consumer_args {
    buffer *buf;
    uint32_t byterate;
    uint32_t count;
    bool debug;
} consumer_args;

typedef struct __producer_args {
    buffer *buf;
    uint32_t unit;
    char *file;
    fops *fops;
    bool debug;
} producer_args;

bool fops_open(fops *fops);
int fops_read(fops *fops, void *buf, int len);
void fops_close(fops *fops);
void fops_free(fops *fops);
fops *fops_posix_new(char *path);
fops *fops_gfapi_new(char *host, int port, char *volume, char *path);

void options_show_help(void);
void options_init(options *opts);
void options_parse(options *opts, int argc, char *argv[]);
void options_validate(options *opts);
void options_free(options *opts);

void *consumer_main(void *arg);
void consumer_init_args(consumer_args *args, buffer *buf, uint32_t byterate, uint32_t count, bool debug);

void *producer_main(void *ptr);
void producer_init_args(producer_args *args, buffer *buf, uint32_t unit, char *file, fops *fops, bool debug);

void buffer_init(buffer *buf, uint32_t size);
void buffer_free(buffer *buf);
void buffer_eof(buffer *buf);
void buffer_wait_producible(buffer *buf);
void buffer_produce(buffer *buf, uint32_t size);
bool buffer_wait_consumable(buffer *buf, uint32_t size, uint64_t *time);
void buffer_consume(buffer *buf, uint32_t size);
uint32_t buffer_get_size(buffer *buf);
uint32_t buffer_get_idx(buffer *buf);
