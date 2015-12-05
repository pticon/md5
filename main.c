#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>


#include "md5.h"


#define VERSION		"1.0"


#define BUFFER_LEN	MD5_CTX_BUFFER_LEN * 100


typedef enum
{
	OPT_UNKNOWN = 0,
	OPT_FILE,
	OPT_STRING,
} process_t;


static void usage(const char * progname)
{
	fprintf(stderr, "Usage: %s [options] <file>...\n", progname);
	fprintf(stderr, "Options:\n");
	fprintf(stderr, "        -h: display this and exit\n");
	fprintf(stderr, "        -v: display version and exit\n");
	fprintf(stderr, "        -s: read from standard input\n");
	fprintf(stderr, "        -p: performance statistics\n");
	fprintf(stderr, "        -q: be quiet\n");
}


static void display_version(const char * progname)
{
	printf("%s: %s\n", progname, VERSION);
}


static void md5_print(const uint8_t * md5)
{
	unsigned i;

	for ( i=0 ; i < MD5_DIGEST_LEN ; i++ )
		printf("%02x", md5[i]);
}


int main(int argc, char * argv[])
{
	md5_ctx_t md5_ctx;
	const char * progname = argv[0];
	const char * string = NULL;
	uint8_t md5[16];
	int opt;
	process_t process = OPT_FILE;
	FILE * file = NULL;
	size_t bytes;
	uint8_t buffer[BUFFER_LEN];
	time_t start = 0, end = 0;
	unsigned long long all = 0;
	int quiet = 0;

	/* Parse arguments
	 */
	while ( (opt = getopt(argc, argv, "hvspq")) != -1 )
	{
		switch (opt)
		{
			case 'h':
			usage(progname);
			return EXIT_SUCCESS;

			case 'v':
			display_version(progname);
			return EXIT_SUCCESS;

			case 's':
			process = OPT_STRING;
			break;

			case 'p':
			if ( time(&start) == ((time_t) -1) )
			{
				perror("time");
				return EXIT_FAILURE;
			}
			break;

			case 'q':
			quiet = 1;
			break;

			default:
			usage(progname);
			return EXIT_FAILURE;
		}
	}

	if ( process == OPT_UNKNOWN )
		process = OPT_FILE;

	if ( optind >= argc )
	{
		usage(progname);
		return EXIT_FAILURE;
	}

	for (; optind < argc; optind++ )
	{
		/* Grab the filename
		 */
		string = argv[optind];

		/* Init the MD5 context
		 */
		md5_init(&md5_ctx);

		/* Compute the hash
		 */
		switch ( process )
		{
			case OPT_STRING:
			md5_update(&md5_ctx, (const uint8_t *) string, strlen(string));
			break;

			case OPT_FILE:
			/* Open it
			 */
			if ( (file = fopen(string, "r")) == NULL )
			{
				perror(string);
				continue;
			}
			/* Read blocks
			 */
			while ( (bytes = fread(buffer, sizeof(uint8_t), BUFFER_LEN, file)) > 0 )
			{
				md5_update(&md5_ctx, buffer, bytes);
				all += bytes;
			}
			/* Check if no error
			 */
			if ( ferror(file) )
			{
				fclose(file);
				perror(string);
				continue;
			}
			fclose(file);
			break;

			case OPT_UNKNOWN:
			fprintf(stderr, "Unknown processing input\n");
			continue;
		}

		/* Finalize the digest
		 */
		md5_final(md5, &md5_ctx);

		/* Display performance stats
		 */
		if ( start != 0 )
		{
			if ( time(&end) == ((time_t) -1) )
			{
				perror("time");
				fprintf(stderr, "No performance statistics to display\n");
			}
			else if (!quiet)
			{
				long diff = (long) (end - start);
				printf("\nTime = %ld seconds\n", diff);
				printf("Speed = %llu bytes/second\n\n", diff > 0 ? (all / diff) : UINT64_MAX );
			}
		}

		/* Print the result
		 */
		md5_print(md5);
		if ( !quiet )
			printf("  %s  %s", string, process == OPT_STRING ? "[string]" : "");
		printf("\n");
	}

	return EXIT_SUCCESS;
}
