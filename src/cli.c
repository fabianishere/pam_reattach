/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 Fabian Mastenbroek
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <getopt.h>

#include <reattach.h>

/**
 * Print the usage message of this program.
 *
 * @param[in] name The name of this program (given by argv[0]).
 */
static void print_usage(char *name) {
	fprintf(stderr, "usage: %s [-u] <program> [args...]\n", name);
	fprintf(stderr, "\t-u --uid\tthe user id of the session to attach to\n");
}

/* Command line options */
static struct option long_options[] = {
	{"uid", required_argument, 0, 'u'},
	{0, 0, 0, 0}
};

/**
 * Main entry point of the command line interface.
 *
 * @param[in] argc The amount of arguments passed to the program.
 * @param[in] argv The command line arguments passed to the program.
 * @return <code>EXIT_SUCCESS</code> on success, <code>EXIT_FAILURE</code>
 * otherwise.
 */
int main(int argc, char **argv)
{
	int err;
	uid_t uid = getuid();

	while (1) {
		int option_index = 0;
		int option = getopt_long (argc, argv, "u:",
			long_options, &option_index);

		if (option == -1) {
			break;
		}

		switch (option) {
		case 0:
			if (long_options[option_index].flag != 0)
				break;
			break;
		case 'u':
			uid = atoi(optarg);
			break;
		case '?':
			print_usage(argv[0]);
			return EXIT_FAILURE;
		default:
			abort();
		}
	}

	if (optind == argc) {
		print_usage(argv[0]);
		return EXIT_FAILURE;
	}

	if ((err = reattach_aqua(uid)) != 0) {
		fprintf(stderr, "reattach_aqua: %d %s\n", err, strerror(err));
		return EXIT_FAILURE;
	}
	execvp(argv[optind], &argv[optind]);

	/* If we reach here, execv failed */
	fprintf(stderr, "execv: %s\n", strerror(errno));
	return EXIT_FAILURE;
}
