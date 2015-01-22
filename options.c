#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "rench.h"

void illegal_option(char *errmsg) {
  fprintf(stderr, "%s\n", errmsg);
  exit(1);
}

void init_options(options *opts) {
  opts->bitrate = DEFAULT_BITRATE;
  opts->file = NULL;
  opts->host = NULL;
  opts->port = -1;
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

void set_host_option(options *opts, char *host) {
  int l = strlen(host);
  if (l == 0) {
    illegal_option("invalid hostname");
  }
  opts->host = malloc(l + 1);
  strcpy(opts->host, host);
}

void set_port_option(options *opts, char *port) {
  int p = atoi(port);
  if (p < 0) {
    illegal_option("port number must be grater than 0.");
  }
  opts->port = p;
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
  while ((opt = getopt(argc, argv, "h:p:f:b:")) != -1) {
    switch (opt) {
      case 'b':
        set_bitrate_option(opts, optarg);
        break;
      case 'h':
        set_host_option(opts, optarg);
        break;
      case 'p':
        set_port_option(opts, optarg);
        break;
      case 'f':
        set_file_option(opts, optarg);
        break;
    }
  }
}

void validate_options(options *opts) {
  if (!opts->host) {
    illegal_option("host is not given.");
  }
  if (opts->port < 0) {
    illegal_option("port is not given.");
  }
  if (!opts->file) {
    illegal_option("file is not given.");
  }
}
