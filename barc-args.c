#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   // for getopt()
#include <string.h>
#include <getopt.h>   //  dito
#include "barc.h"
#include "gitversion.h"

const char *barc_version = "0.0.1";

void print_usage(char*const *argv)
{
    printf("parse for .barcode requests and generate barcode as troff primitives\n\n");
    printf("   usage: %s [OPTIONS]\n\n", argv[0]);
    printf("   -h --help          help\n");
    printf("   -q --quiet         suppress messages on stderr\n");
    printf("   -d --debug         print debug messages on stderr\n");
    printf("   -a --addon-char    define addon char (default \";\")\n");
    printf("   -C --no-checksum   ignore failed checksum test and enforce output\n");
    printf("   -v --version       show programm version\n");
    printf("   -f --open [FILE|-] open file instead of stdin\n");
    printf("   -o --out  [FILE|-] open file instead of stdout\n");
}

static const char *optString = "hqda:Cvf:o:";
static struct option long_options[] =
{
    {"help",        no_argument,       NULL, 'h'},
    {"quiet",       no_argument,       NULL, 'q'},
    {"debug",       no_argument,       NULL, 'd'},
    {"addon-char",  required_argument, NULL, 'a'},
    {"no-checksum", no_argument,       NULL, 'C'},
    {"version",     no_argument,       NULL, 'v'},
    {"open",        required_argument, NULL, 'f'},
    {"out",         required_argument, NULL, 'o'},
    {NULL,          0,                 NULL,  0 }
};

void
parse_args(int *argc_p, char*const *argv, struct options *o)
{
    int c;
    int option_index;
    while ((c=getopt_long(*argc_p, argv, optString, long_options, &option_index)) != -1)
	switch(c)
	{
// print different messages/error messages without generating a barcode
	case 'v':
	  printf("Barcode Generator %s Version %s (Git-Version %s)\n", argv[0], barc_version, gitversion);
	  printf("Parse for .barcode requests and generate barcode as troff primitives.\n");
	  exit(0);
	case '?':
	    print_usage(argv);
	    abort();
	case 'h':
	    print_usage(argv);
	    exit(0);

// set flags for quiet mode, debug mode, no-checksum mode
	case 'q':
	    o->quiet=1;
	    break;
	case 'd':
	    o->debug=1;
	    break;
	case 'C':
	    o->no_checksum=1;
	    break;

// read separator character
        case 'a':
	    if (optarg !=NULL && optarg != 0)
	    {
		o->aoc = (unsigned char) optarg[0];
		break;
	    }
	    else
		printf("Error: Missing separator character!\n");
	    exit(1);

// set input and output files
	case 'f':
	    exit(1);
	case 'o':
	    exit(1);

// on bad option print usage message and abort
	default:
	    print_usage(argv);
	    abort();
	}
}

