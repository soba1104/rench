#include "rench.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void illegal_option(char *errmsg) {
    fprintf(stderr, "%s\n", errmsg);
    exit(1);
}

void options_show_help(void) {
    fprintf(stdout, "--------------------- general options ---------------------\n");
    fprintf(stdout, "-s bufsize:     buffer size. default 10M.\n");
    fprintf(stdout, "-b bitrate:     bitrate. default 1M.\n");
    fprintf(stdout, "-B byterate:    byterate. default 128K.\n");
    fprintf(stdout, "-u upper:       maximum bytes per read. default 128K.\n");
    fprintf(stdout, "-l lower:       minimum bytes per read. default 128K.\n");
    fprintf(stdout, "-t type:        fops type(posix or gfapi). default posix.\n");
    fprintf(stdout, "-c count:       count to read. read entire file by default.\n");
    fprintf(stdout, "-C concurrency: concurrency. default 1.\n");
    fprintf(stdout, "-d:             debug mode. default off.\n");
    fprintf(stdout, "-----------------------------------------------------------\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "---------------------- gfapi options ----------------------\n");
    fprintf(stdout, "-h:           hostname.\n");
    fprintf(stdout, "-p:           port number.\n");
    fprintf(stdout, "-v:           volume name.\n");
    fprintf(stdout, "-----------------------------------------------------------\n");
}

void options_init(options *opts) {
    opts->byterate = DEFAULT_BYTERATE;
    opts->bufsize = DEFAULT_BUFSIZE;
    opts->upper = DEFAULT_BYTERATE;
    opts->lower = DEFAULT_BYTERATE;
    opts->host = NULL;
    opts->port = -1;
    opts->volume = NULL;
    opts->type = POSIX;
    opts->debug = false;
    opts->count = 0;
    opts->concurrency = 1;
}

void set_byterate_option(options *opts, char *byterate) {
    int l = strlen(byterate);
    int b = atoi(byterate);
    if (b <= 0) {
        illegal_option("byterate must be greater than 0.");
    }
    switch (byterate[l - 1]) {
        case 'M':
        case 'm':
            b *= 1024;
        case 'k':
        case 'K':
            b *= 1024;
    }
    opts->byterate = b;
}

void set_bitrate_option(options *opts, char *bitrate) {
    int l = strlen(bitrate);
    int b = atoi(bitrate);
    switch (bitrate[l - 1]) {
        case 'M':
        case 'm':
            b *= 1024;
        case 'k':
        case 'K':
            b *= 1024;
    }
    if (b < 8) {
        illegal_option("bitrate must be greater than or equal to 8.");
    }
    opts->byterate = b / 8;
}

void set_bufsize_option(options *opts, char *bufsize) {
    int l = strlen(bufsize);
    int s = atoi(bufsize);
    if (s <= 0) {
        illegal_option("buffer size must be greater than 0.");
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

void set_count_option(options *opts, char *count) {
    int l = strlen(count);
    int c = atoi(count);
    if (c <= 0) {
        illegal_option("count must be greater than 0.");
    }
    opts->count = c;
}

void set_upper_option(options *opts, char *upper) {
    int l = strlen(upper);
    int u = atoi(upper);
    if (u <= 0) {
        illegal_option("upper bound must be greater than 0.");
    }
    switch (upper[l - 1]) {
        case 'M':
        case 'm':
            u *= 1024;
        case 'k':
        case 'K':
            u *= 1024;
    }
    opts->upper = u;
}

void set_lower_option(options *opts, char *lower) {
    int l = strlen(lower);
    int u = atoi(lower);
    if (u <= 0) {
        illegal_option("lower bound must be greater than 0.");
    }
    switch (lower[l - 1]) {
        case 'M':
        case 'm':
            u *= 1024;
        case 'k':
        case 'K':
            u *= 1024;
    }
    opts->lower = u;
}

void set_concurrency_option(options *opts, char *concurrency) {
    int l = strlen(concurrency);
    int c = atoi(concurrency);
    if (c <= 0) {
        illegal_option("concurrency must be greater than 0.");
    }
    opts->concurrency = c;
}

void set_host_option(options *opts, char *host) {
    int l = strlen(host);
    if (l == 0) {
        illegal_option("invalid hostname.");
    }
    opts->host = malloc(l + 1);
    strcpy(opts->host, host);
}

void set_port_option(options *opts, char *port) {
    int p = atoi(port);
    if (p < 0) {
        illegal_option("port number must be greater than or equal to 0.");
    }
    if (p > 65535) {
        illegal_option("port number must be less than 65536.");
    }
    opts->port = p;
}

void set_volume_option(options *opts, char *volume) {
    int l = strlen(volume);
    if (l == 0) {
        illegal_option("invalid volume name.");
    }
    opts->volume = malloc(l + 1);
    strcpy(opts->volume, volume);
}

void set_fops_type_option(options *opts, char *type) {
    if (strcmp(type, "posix") == 0) {
        opts->type = POSIX;
    } else if (strcmp(type, "gfapi") == 0) {
        opts->type = GFAPI;
    } else {
        illegal_option("invalid fops type.");
    }
}

void set_debug_option(options *opts) {
    opts->debug = true;
}

void options_parse(options *opts, int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "s:u:l:b:B:h:p:t:v:c:C:d")) != -1) {
        switch (opt) {
            case 'B':
                set_byterate_option(opts, optarg);
                break;
            case 'b':
                set_bitrate_option(opts, optarg);
                break;
            case 's':
                set_bufsize_option(opts, optarg);
                break;
            case 'c':
                set_count_option(opts, optarg);
                break;
            case 'u':
                set_upper_option(opts, optarg);
                break;
            case 'l':
                set_lower_option(opts, optarg);
                break;
            case 'C':
                set_concurrency_option(opts, optarg);
                break;
            case 'h':
                set_host_option(opts, optarg);
                break;
            case 'p':
                set_port_option(opts, optarg);
                break;
            case 'v':
                set_volume_option(opts, optarg);
                break;
            case 't':
                set_fops_type_option(opts, optarg);
                break;
            case 'd':
                set_debug_option(opts);
                break;
        }
    }
}

void options_validate(options *opts) {
    if (opts->type == GFAPI) {
        if (!opts->host) {
            illegal_option("hostname is not given.");
        }
        if (opts->port < 0) {
            illegal_option("port number is not given.");
        }
        if (!opts->volume) {
            illegal_option("volume name is not given.");
        }
    }
    if (opts->upper > opts->bufsize) {
        illegal_option("read unit upper bound should not be greater than buffer size.");
    }
    if (opts->lower > opts->upper) {
        illegal_option("read unit lower bound should not be greater than upper bound.");
    }
}

void options_free(options *opts) {
    if (opts->host) {
        free(opts->host);
    }
    if (opts->volume) {
        free(opts->volume);
    }
}
