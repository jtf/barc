#include <stdio.h>
#include <string.h>   // for strlen()
#include <ctype.h>    // for isspace(), isblank()
#include "barc.h"
#include "barc-ean.h"



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
parse_barcode_type(char ** barcode, struct barcode_data *bc)
{
  char * bc_type;

  const char type_string[5] = "ISBN\0";

  // test just for ISBN for now
  bc_type = strstr(*barcode, type_string);
  //    fprintf(stderr, "BARCODE: CODE: %s \n", bc_type);

  if (bc_type != NULL)
    {
      bc_type += 5;
      ignore_leading_blank(&bc_type);
      //	fprintf(stderr, "BARCODE: type: \"%s\" \n", bc_type);
      return parse_ISBN(&bc_type, bc);
      //	return 0;
    }
  else
    {
      return 0;
    }
}

int
parse_request(char ** request, int row)
{
  char *content, *err_output_p;
  //    FILE file;

  const char bc_string[] = "barcode\0";
  content = strstr(*request, bc_string);
  //    fprintf(stderr, "BARCODE: REQUEST: c  \"%s\"\n", content);

  if (content != NULL)
    {
      content += strlen(bc_string) +1;

      // debug output
      fprintf(stderr, "BARCODE request found in line (%d) with parameters: \"", row);
      err_output_p = content;
      while(*err_output_p != '\n')
	{
	  fputc(*err_output_p, stderr);
	  err_output_p++;
	}
      fprintf(stderr, "\"\n");

      *request = content;

      //	if (
      return parse_barcode_type(&content, &bc_data);
      //	)
      //	{
      //	    return 0;
      //	}
      //	else
      //	{
      //	    return 1;
      //	}

      //	fprintf(stderr, "BARCODE: REQUEST: *r %s\n", *request);
      //	return 0;
    }
  else
    {
      return 0;
    }
}


int
main()
{
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

	  if (parse_request(&request_name_p, row))
	    {

	      // Barcode erfolgreich analysiert .. keine Fehler
	      // wir können ihn generieren

	      //		fprintf(stdout, "Barcode [%d, %s, %d]\n", bc_data.barcode_type, bc_data.title, bc_data.checksum);

	      //		fprintf(stdout, "[Barcode: %s]\n", bc_data.title);
	      //		print_foo();
	      fprintf(stderr, "Bytes read: %d\n", bytenum);
	      fill_EAN_13(&bc_data);
	      print_EAN_13(&bc_data);
	    }
	  else
	    {
	      // write out original request
	      fprintf(stderr, "║│!║││║ ! [request ignored]\n");
	      fputc(c, stdout);
	      fputs(request_buffer, stdout);
	    }
	  column=1;
	  row++;
	  continue;	// skip write through
	}
      // write through
      putc(c, stdout);
      column++;
    }

  return 0;
}
