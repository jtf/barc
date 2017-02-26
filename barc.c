#include <stdio.h>
#include <string.h>   // for strlen()
#include <ctype.h>    // for isspace(), isblank()
#include "barc.h"
#include "barc-args.h"
#include "barc-ean.h"


//struct options Opts = {0};
struct barcode_data bc_data;

//-----------------------------------------------------------------------------

void
print_foo()
{
  fprintf(stdout, "Menno");
}

//-----------------------------------------------------------------------------
void
ignore_leading_blank(char **str)
{
  while(isblank((unsigned char) **str)) (*str)++;
}



int
parse_barcode_type(char ** barcode, struct barcode_data *bc, struct options *o)
{
  char * bc_type;

  const char type_string[5] = "ISBN\0";

  // test just for ISBN for now
  bc_type = strstr(*barcode, type_string);

  if (bc_type != NULL)
    {
      bc_type += 5;
      ignore_leading_blank(&bc_type);
      return parse_ISBN(&bc_type, bc, o);
     }
  else
    {
      return 0;
    }
}

int
parse_request(char ** request, int row, struct options *o)
{
  
  char *content, *err_output_p;
  //    FILE file;

  const char bc_string[] = "barcode\0";
  content = strstr(*request, bc_string);

  if (content != NULL)
    {
      content += strlen(bc_string) +1;

      // debug output
      if (!o->quiet)
      {
	  fprintf(stderr, "BARCODE request found in line (%d) with parameters: \"", row);
	  
	  err_output_p = content;
	  while(*err_output_p != '\n')
	  {
	      fputc(*err_output_p, stderr);
	      err_output_p++;
	  }
	  fprintf(stderr, "\"\n");
      }
      *request = content;

      return parse_barcode_type(&content, &bc_data, o);
    }
  else
    {
      return 0;
    }
}

int
main(int argc, char **argv)
{

    struct options Opts = {0};
    
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
	  
	    if (parse_request(&request_name_p, row, &Opts))
	    {

		// Barcode erfolgreich analysiert .. keine Fehler
		// wir können ihn generieren
		
                // fprintf(stdout, "Barcode [%d, %s, %d]\n", bc_data.barcode_type, bc_data.title, bc_data.checksum);
		// fprintf(stdout, "[Barcode: %s]\n", bc_data.title);
		// print_foo();
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
