#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>

#define DEFAULT_BITRATE (128 * 1024)

typedef struct __options {
	uint32_t bitrate;
} options;

void illegal_option(char *errmsg) {
	fprintf(stderr, "%s\n", errmsg);
	exit(1);
}

void init_options(options *opts) {
	opts->bitrate = DEFAULT_BITRATE;
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

void parse_options(options *opts, int argc, char *argv[]) {
	int opt;
	while ((opt = getopt(argc, argv, "b:")) != -1) {
		switch (opt) {
			case 'b':
				set_bitrate_option(opts, optarg);
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
