#include "rench.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <glusterfs/api/glfs.h>

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

void fops_posix_free(void *arg) {
    // nothing to do
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
    fops_posix->free = fops_posix_free;
    fops_posix->state = state;
    state->path = path;
    state->fd = -1;

    return fops_posix;
}

typedef struct __fops_gfapi_state {
    char *path;
    glfs_t *fs;
    glfs_fd_t *fd;
} fops_gfapi_state;

bool fops_gfapi_open(void *arg) {
    fops_gfapi_state *state = arg;
    glfs_t *fs = state->fs;
    char *path = state->path;

    if (!glfs_init(fs)) {
        return false;
    }
    state->fd = glfs_open(fs, path, O_RDONLY); // FIXME

    return state->fd != NULL;
}

int fops_gfapi_read(void *arg, void *buf, int len) {
    fops_gfapi_state *state = arg;
    return glfs_read(state->fd, buf, len, 0);
}

void fops_gfapi_close(void *arg) {
    fops_gfapi_state *state = arg;
    if (state->fd) {
        glfs_close(state->fd);
    }
}

void fops_gfapi_free(void *arg) {
    fops_gfapi_state *state = arg;
    if (state->fs) {
        glfs_fini(state->fs);
    }
}

fops *fops_gfapi_new(char *host, int port, char *volume, char *path) {
    fops *fops_gfapi;
    fops_gfapi_state *state;

    fops_gfapi = malloc(sizeof(fops) + sizeof(fops_gfapi_state));
    if (!fops_gfapi) {
        goto err;
    }
    state = (fops_gfapi_state*)(fops_gfapi + 1);
    fops_gfapi->open = fops_gfapi_open;
    fops_gfapi->read = fops_gfapi_read;
    fops_gfapi->close = fops_gfapi_close;
    fops_gfapi->free = fops_gfapi_free;
    fops_gfapi->state = state;
    state->path = path;
    state->fd = NULL;
    state->fs = glfs_new(volume);
    if (!state->fs) {
        goto err;
    }
    glfs_set_volfile_server(state->fs, "tcp", host, port);
    return fops_gfapi;

err:
    if (fops_gfapi) {
        fops_free(fops_gfapi);
    }
    return NULL;
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
    fops->free(fops->state);
    free(fops);
}
