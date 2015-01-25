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
    fprintf(stdout, "------------------- general options -------------------\n");
    fprintf(stdout, "-f path:      (requirement)target file path.\n");
    fprintf(stdout, "-s bufsize:   buffer size. default 10M.\n");
    fprintf(stdout, "-b byterate:  byterate. default 128K.\n");
    fprintf(stdout, "-t type:      fops type(posix or gfapi). default posix.\n");
    fprintf(stdout, "-d:           debug mode. default off.\n");
    fprintf(stdout, "-------------------------------------------------------\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "-------------------- gfapi options --------------------\n");
    fprintf(stdout, "-h:           hostname.\n");
    fprintf(stdout, "-p:           port number.\n");
    fprintf(stdout, "-v:           volume name.\n");
    fprintf(stdout, "-------------------------------------------------------\n");
}

void options_init(options *opts) {
    opts->byterate = DEFAULT_BYTERATE;
    opts->bufsize = DEFAULT_BUFSIZE;
    opts->file = NULL;
    opts->host = NULL;
    opts->port = -1;
    opts->volume = NULL;
    opts->type = UNDEF;
    opts->debug = false;
}

void set_byterate_option(options *opts, char *byterate) {
    int l = strlen(byterate);
    int b = atoi(byterate);
    if (b <= 0) {
        illegal_option("byterate must be grater than 0.");
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
        illegal_option("invalid filepath.");
    }
    opts->file = malloc(l + 1);
    strcpy(opts->file, file);
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
    } else {
        illegal_option("invalid fops type.");
    }
}

void set_debug_option(options *opts) {
    opts->debug = true;
}

void options_parse(options *opts, int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "s:f:b:h:p:t:v:d")) != -1) {
        switch (opt) {
            case 'b':
                set_byterate_option(opts, optarg);
                break;
            case 's':
                set_bufsize_option(opts, optarg);
                break;
            case 'f':
                set_file_option(opts, optarg);
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
    if (!opts->file) {
        illegal_option("file is not given.");
    }
    if (opts->type == UNDEF) {
        illegal_option("fops type is not given.");
    }
}

void options_free(options *opts) {
    if (opts->file) {
        free(opts->file);
    }
    if (opts->host) {
        free(opts->host);
    }
    if (opts->volume) {
        free(opts->volume);
    }
}
