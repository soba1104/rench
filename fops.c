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
    // nothing to do
}

fops *fops_gfapi_new(glfs_t *fs, char *path) {
    fops *fops_gfapi;
    fops_gfapi_state *state;

    fops_gfapi = malloc(sizeof(fops) + sizeof(fops_gfapi_state));
    if (!fops_gfapi) {
        return NULL;
    }
    state = (fops_gfapi_state*)(fops_gfapi + 1);
    fops_gfapi->open = fops_gfapi_open;
    fops_gfapi->read = fops_gfapi_read;
    fops_gfapi->close = fops_gfapi_close;
    fops_gfapi->free = fops_gfapi_free;
    fops_gfapi->state = state;
    state->path = path;
    state->fd = NULL;
    state->fs = fs;
    return fops_gfapi;
}

typedef struct __fops_rados_state {
    char *objname;
    rados_ioctx_t ioctx;
    int idx;
} fops_rados_state;

bool fops_rados_open(void *arg) {
    char buf[1];
    fops_rados_state *state = arg;
    return rados_read(state->ioctx, state->objname, buf, 1, 0) == 1;
}

int fops_rados_read(void *arg, void *buf, int len) {
    fops_rados_state *state = arg;
    int ret = rados_read(state->ioctx, state->objname, buf, len, state->idx);
    if (ret > 0) {
        state->idx += ret;
    }
    return ret;
}

void fops_rados_close(void *arg) {
    // nothing to do
}

void fops_rados_free(void *arg) {
    // nothing to do
}

fops *fops_rados_new(rados_ioctx_t *ioctx, char *objname) {
    fops *fops_rados;
    fops_rados_state *state;

    fops_rados = malloc(sizeof(fops) + sizeof(fops_rados_state));
    if (!fops_rados) {
        return NULL;
    }
    state = (fops_rados_state*)(fops_rados + 1);
    fops_rados->open = fops_rados_open;
    fops_rados->read = fops_rados_read;
    fops_rados->close = fops_rados_close;
    fops_rados->free = fops_rados_free;
    fops_rados->state = state;
    state->objname = objname;
    state->ioctx = ioctx;
    state->idx = 0;
    return fops_rados;
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

fops *fops_new(char *file, options *opts, glfs_t *glfs, rados_ioctx_t rados_ioctx) {
    fops *fops;
    switch (opts->type) {
        case POSIX:
            fops = fops_posix_new(file);
            if (opts->debug) {
                fprintf(stdout,
                        "type = posix, file = %s, byterate = %u, upper = %u, lower = %u, bufsize = %u\n",
                        file, opts->byterate, opts->upper, opts->lower, opts->bufsize);
            }
            break;
        case GFAPI:
            fops = fops_gfapi_new(glfs, file);
            if (opts->debug) {
                fprintf(stdout,
                        "type = gfapi, host = %s, port = %d, volume = %s, file = %s, byterate = %u, upper = %u, lower = %u, bufsize = %u\n",
                        opts->host, opts->port, opts->volume, file, opts->byterate, opts->upper, opts->lower, opts->bufsize);
            }
            break;
        case RADOS:
            fops = fops_rados_new(rados_ioctx, file);
            if (opts->debug) {
                fprintf(stdout,
                        "type = rados, cluster name = %s, pool name = %d, config path = %s, user name = %s, file = %s, byterate = %u, upper = %u, lower = %u, bufsize = %u\n",
                        opts->rados_cluster_name, opts->rados_pool_name, opts->rados_config_path, opts->rados_user_name, file, opts->byterate, opts->upper, opts->lower, opts->bufsize);
            }
            break;
    }
    return fops;
}
