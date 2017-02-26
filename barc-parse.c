#include <stdio.h>    // for fprintf
#include <string.h>   // for strlen()
#include <ctype.h>    // for isspace(), isblank()
#include "barc.h"
#include "barc-ean.h"

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
parse_request(char ** request, int row, struct barcode_data *bc, struct options *o)
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

      return parse_barcode_type(&content, bc, o);
    }
  else
    {
      return 0;
    }
}
