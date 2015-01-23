#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/types.h>
#include <sys/socket.h>

#define DEFAULT_BITRATE (128 * 1024)

typedef struct __options {
	uint32_t bitrate;
	char *host;
	int port;
} options;

void illegal_option(char *errmsg) {
	fprintf(stderr, "%s\n", errmsg);
	exit(1);
}

void init_options(options *opts) {
	opts->bitrate = DEFAULT_BITRATE;
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
}

void parse_options(options *opts, int argc, char *argv[]) {
	int opt;
	while ((opt = getopt(argc, argv, "h:p:b:")) != -1) {
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
		}
	}
}

int main(int argc, char *argv[]) {
	options opts;

	init_options(&opts);
	parse_options(&opts, argc, argv);

	fprintf(stderr, "bitrate = %u\n", opts.bitrate);

	return 0;
}