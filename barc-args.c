#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   // for getopt()
#include <getopt.h>   //  dito
#include "barc.h"

//TODO: width, height, box
//TODO: .. ISBN title option (default: none)

void print_usage(char*const *argv)
{
    printf("parse for .barcode requests and generate barcode as troff primitives\n\n");
    printf("   usage: %s [OPTIONS]\n\n", argv[0]);
    printf("   -h --help          help\n");
    printf("   -q --quiet         suppress messages on stderr\n");
    printf("   -a --addon-char    define addon char (default \";\")\n");
    printf("   -C --no-checksum   ignore failed checksum test and enforce output\n");
    printf("   -v --version       show programm version\n");
    printf("   -f --open [FILE|-] open file instead of stdin\n");
    printf("   -o --out  [FILE|-] open file instead of stdout\n");
}

static const char *optString = "hqvf:oC";
static struct option long_options[] =
{
    {"help",        no_argument,       NULL, 'h'},
    {"quiet",       no_argument,       NULL, 'q'},
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
	case 'q':
	    o->quiet=1;
	    break;
	case 'C':
	    o->no_checksum=1;
	    break;
        case 'a':
	    o->aoc = ';';  //TODO
	    break;
	case 'f':
	    exit(1);
	case '?':
	    print_usage(argv);
	    abort();
	case 'h':
	default:
	    print_usage(argv);
	    exit(0);
	    abort();
	}
}

