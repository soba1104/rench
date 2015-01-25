#include "rench.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct __fops_posix_state {
    char *path;
    int fd;
} fops_posix_state;

bool fops_posix_open(void *arg) {
    fops_posix_state *state = arg;
    char *path = state->path;
    state->fd = open(path, O_RDONLY); // FIXME
    return state->fd >= 0;
}

int fops_posix_read(void *arg, void *buf, int len) {
    fops_posix_state *state = arg;
    return read(state->fd, buf, len);
}

void fops_posix_close(void *arg) {
    fops_posix_state *state = arg;
    if (state->fd >= 0) {
        close(state->fd);
    }
}

fops *fops_posix_new(char *path) {
    fops *fops_posix;
    fops_posix_state *state;

    fops_posix = malloc(sizeof(fops) + sizeof(fops_posix_state));
    if (!fops_posix) {
        return NULL;
    }
    state = (fops_posix_state*)(fops_posix + 1);
    fops_posix->open = fops_posix_open;
    fops_posix->read = fops_posix_read;
    fops_posix->close = fops_posix_close;
    fops_posix->state = state;
    state->path = path;
    state->fd = -1;

    return fops_posix;
}

bool fops_open(fops *fops) {
    return fops->open(fops->state);
}

int fops_read(fops *fops, void *buf, int len) {
    return fops->read(fops->state, buf, len);
}

void fops_close(fops *fops) {
    fops->close(fops->state);
}

void fops_free(fops *fops) {
    free(fops);
}
