#include <stdio.h>
#include <string.h>   // for strlen()
#include <ctype.h>    // for isspace(), isblank()
#include "barc.h"
#include "barc-parse.h"
#include "barc-ean.h"

int
main(int argc, char **argv)
{
    struct options Opts = {0, 0};
    struct barcode_data bc_data;
    
    parse_args(&argc, argv, &Opts);

    int c;
    unsigned int column  = 1;
    unsigned int row     = 1;
    unsigned int bytenum = 0;
    char request_buffer[1000];
    char *request_name_p;
    
    while ((c=fgetc(stdin)) != EOF)
    {
	bytenum++;

	if (c=='\n')
	{
	    column=0;
	    row++;
	}
	else if (c=='.' && column==1)
	{
	    
	    // read request, check for spaces and tabs and ignore them,
	    // 
	    fgets(request_buffer, 1000, stdin);
	    bytenum += strlen(request_buffer);
	    request_name_p = request_buffer;
	    ignore_leading_blank(&request_name_p);

	    //reset barcode
	    for(int l=0; l<=14; l++)
		bc_data.UPC[l]=0;
	    bc_data.barcode_type = undefined;
		
	    if (parse_request(&request_name_p, row, &bc_data, &Opts))
	    {

		// Barcode erfolgreich analysiert .. keine Fehler
		// wir können ihn generieren
		
                // fprintf(stdout, "Barcode [%d, %s, %d]\n", bc_data.barcode_type, bc_data.title, bc_data.checksum);

		fill_EAN_13(&bc_data);
		print_EAN_13(&bc_data);
	    }
	    else
	    {
		// write out original request
		if (!Opts.quiet)
		{
		    fprintf(stderr, "║│!║││║ ! [request ignored]\n");
		    fputc(c, stdout);
		    fputs(request_buffer, stdout);
		}
	    }
	    column=1;
	    row++;
	    continue;	// skip write through
	}
	// write through
	putc(c, stdout);
	column++;
    }
    if (!Opts.quiet) fprintf(stderr, "Bytes read: %d\n", bytenum);

    return 0;
}
