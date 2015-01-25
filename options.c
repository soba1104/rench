#include "rench.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void illegal_option(char *errmsg) {
    fprintf(stderr, "%s\n", errmsg);
    exit(1);
}

void init_options(options *opts) {
    opts->bitrate = DEFAULT_BITRATE;
    opts->bufsize = DEFAULT_BUFSIZE;
    opts->file = NULL;
}

void set_bitrate_option(options *opts, char *bitrate) {
    int l = strlen(bitrate);
    int b = atoi(bitrate);
    if (b <= 0) {
        illegal_option("bitrate must be grater than 0.");
    }
    switch (bitrate[l - 1]) {
        case 'M':
        case 'm':
            b *= 1024;
        case 'k':
        case 'K':
            b *= 1024;
    }
    opts->bitrate = b;
}

void set_bufsize_option(options *opts, char *bufsize) {
    int l = strlen(bufsize);
    int s = atoi(bufsize);
    if (s <= 0) {
        illegal_option("buffer size must be grater than 0.");
    }
    switch (bufsize[l - 1]) {
        case 'M':
        case 'm':
            s *= 1024;
        case 'k':
        case 'K':
            s *= 1024;
    }
    opts->bufsize = s;
}

void set_file_option(options *opts, char *file) {
    int l = strlen(file);
    if (l == 0) {
        illegal_option("invalid filepath");
    }
    opts->file = malloc(l + 1);
    strcpy(opts->file, file);
}

void parse_options(options *opts, int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "s:f:b:")) != -1) {
        switch (opt) {
            case 'b':
                set_bitrate_option(opts, optarg);
                break;
            case 's':
                set_bufsize_option(opts, optarg);
                break;
            case 'f':
                set_file_option(opts, optarg);
                break;
        }
    }
}

void validate_options(options *opts) {
    if (!opts->file) {
        illegal_option("file is not given.");
    }
}

void free_options(options *opts) {
    if (opts->file) {
        free(opts->file);
    }
}
