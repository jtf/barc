#include <stdio.h>
#include <ctype.h>    // for isdigit()
//#include <string.h>   // for strlen()
#include "barc.h"
#include "barc-ean.h"


// calculate checksum depending on barcode type
// return 0 if checksum differs from input else 1
int
calc_ean_checksum(struct barcode_data *bc)
{
    int rv; //return value
    bc->checksum = 0;  // init checksum

    switch(bc->barcode_type)
    {
    case ISBN_10:
	for (int i=0; i<9; i++)
	{
	    bc->checksum += bc->UPC[i]*(i+1);
	}
	bc->checksum %= 11;
	rv = (bc->UPC[9] == bc->checksum);
	break;
    case EAN_13:
    case ISBN_13:
	for (int i=0; i<6; i++)
	{
	    bc->checksum += bc->UPC[2*i] + bc->UPC[2*i+1]*3;
	}
	bc->checksum = (10 - (bc->checksum % 10)) % 10;
	rv = (bc->UPC[12] == bc->checksum);
	break;
    }
    return rv;
}

void
transform_ISBN10to13(struct barcode_data *bc)
{
    if (bc->barcode_type != ISBN_10) return;

    // arrange numbers
    for (int i=8; i>=0; i--)
	bc->UPC[i+3] = bc->UPC[i];
    bc->UPC[0]=9;
    bc->UPC[1]=7;
    bc->UPC[2]=8;

    // set new type
    bc->barcode_type = ISBN_13;

    // calculate and reset new checksum
    if (!calc_ean_checksum(bc))
	bc->UPC[12] = bc->checksum;
}

// extern: Parse ISBN Number and save values in barcode struct
int
parse_ISBN(char **isbn, struct barcode_data *bc, struct options *o)
{
    int isbnpos = 0;
    char * digit_p;
    unsigned char * title_p;

    digit_p = *isbn;

    title_p = bc->title;

    // let title begin with "ISBN "
    *title_p++='I'; *title_p++='S'; *title_p++='B'; *title_p++='N'; *title_p++=' ';

    if (!o->quiet) fprintf(stderr, "║│ ║││║   ISBN: ");

    while( *digit_p != '\n' && isbnpos <= 13 )
    {
	*title_p++ = *digit_p;
	if (isdigit(*digit_p))
	{
	    bc->UPC[isbnpos++] = *digit_p - 48;
	    if (!o->quiet) fprintf(stderr, "%c ", *digit_p);
	}
	else
	    if (isbnpos==9 && ( *digit_p == 'X' || *digit_p=='x'))
	    {
		if (!o->quiet) fprintf(stderr, "%c (X found) ", *digit_p);
		bc->UPC[isbnpos++] = 10;
	    }
	digit_p++;
    }
    if (!o->quiet) fprintf(stderr, "\n");

    *title_p = '\0';

    // decide on number of digits which code we have
    switch(isbnpos)
    {
	// ------------------- ISBN 10 -------------------------
    case 10 :
	if (!o->quiet) fprintf(stderr, "║│ ║││║   found ISBN-10\n║│ ║││║   1-14: ");
	for(int i=0; i<14; i++)
	{
	    if (!o->quiet) fprintf(stderr, "%d ", bc->UPC[i]);
	}
	bc->barcode_type=ISBN_10;
	if (!o->quiet) fprintf(stderr, "\n");

	if(calc_ean_checksum(bc))
	{
	    if (!o->quiet) fprintf(stderr, "║│ ║││║      calculated checksum: %d\n", bc->checksum);

	    // transform ISBN-10 to ISBN-13
	    transform_ISBN10to13(bc);

	    // output message
	    if (!o->quiet)
	    {
		fprintf(stderr, "║│ ║││║   transformed to ISBN-13\n");
		fprintf(stderr, "║│ ║││║   1-14: ");
		for(int i=0; i<14; i++)
		    fprintf(stderr, "%d ", bc->UPC[i]);
		fprintf(stderr, "\n");
		fprintf(stderr, "║│ ║││║        new calculated checksum: %d\n", bc->checksum);
	    }

	}
	else
	{
	    if (!o->quiet) fprintf(stderr, "║│!║││║   ISBN-10 checksum error  ^ (should be %d, is %d)\n",
				   bc->checksum, bc->UPC[9]);
	    return 0;
	}
	break;
	// ------------------- ISBN 13 -------------------------
    case 13 :
	if (!o->quiet) fprintf(stderr, "║│ ║││║   found ISBN-13\n║│ ║││║   1-14: ");
	for(int i=0; i<14; i++)
	{
	    if (!o->quiet) fprintf(stderr, "%d ", bc->UPC[i]);
	}
	bc->barcode_type=ISBN_13;
	if (!o->quiet) fprintf(stderr, "\n");

	if(calc_ean_checksum(bc))
	{
	    if (!o->quiet) fprintf(stderr, "║│ ║││║            calculated checksum: %d\n", bc->checksum);
	}
	else
	{
	    if (!o->quiet) fprintf(stderr, "║│!║││║   ISBN-13 checksum error        ^ (should be %d, is %d)\n",
				   bc->checksum, bc->UPC[12]);
	    return 0;
	}
	break;
    default :
	if (!o->quiet) fprintf(stderr, "║│!║││║   Error parsing ISBN (too short?)\n");
	return 0;
    }
    return 1;
}

// local: insert bar lengths of given guard pattern in barcode struct
int
insert_guard(int pos, int guard_type, struct barcode_data *bc)
{
  for (int i=1; i<=ean_delimiter[guard_type][0]; i++)
    bc->bars[pos++] = ean_delimiter[guard_type][i];
  return pos;
}

// local: insert bar lengths of left 6 digits in barcode struct
int
insert_left_digits(int pos, struct barcode_data *bc)
{
  for (int digit=0; digit<6; digit++)
    for (int modul=0; modul<4; modul++)
      bc->bars[pos++] =
	ean_symbols
	[ bc->UPC[digit+1] ]
	[ean_symbol_direction
	 [ean13_leftmost_order
	  [bc->UPC[0]]
	  [digit]
	  ]
	 [modul]];
  return pos;
}

// local: insert bar lengths of right 6 digits in barcode struct
int
insert_right_digits(int pos, struct barcode_data *bc)
{
  for (int digit=0; digit<6; digit++)
    for (int modul=0; modul<4; modul++)
      bc->bars[pos++] =
	ean_symbols[bc->UPC[digit+7]]
	[ean_symbol_direction[EAN_A][modul]];
  return pos;
}

/*
 *  take barcode struct and fill .bars with bar lengths
 */
void
fill_EAN_13(struct barcode_data *bc)
{
  int pos = 0;
  pos = insert_guard(pos, EAN_NORMAL_GUARD, bc);
  pos = insert_left_digits(pos, bc);
  pos = insert_guard(pos, EAN_CENTRE_GUARD, bc);
  pos = insert_right_digits(pos, bc);
  pos = insert_guard(pos, EAN_NORMAL_GUARD, bc);
  pos = 0;
}

/*
 *  take barcode struct and generate TROFF code from bc.bars
 */
void
print_EAN_13(struct barcode_data *bc)
{
  int longtoggler = 0;

  double width   = 0.033f;
  double height  = 2.285f;
  double lheight = 2.450f;

  //define macros
  fprintf(stdout, ".do xflag 3\n");
  fprintf(stdout, ".de black\n\\\\D'P\\\\$1 0v 0m %.4fc -\\\\$1 -0v 0c -%.4fc'\\h'\\\\$1'\\c\n..\n", height, height);
  fprintf(stdout, ".de blacklong\n\\\\D'P\\\\$1 0v 0m %.4fc -\\\\$1 -0v 0c -%.4fc'\\h'\\\\$1'\\c\n..\n", lheight, lheight);
  fprintf(stdout, ".de space\n\\\\h'\\\\$1'\\c\n..\n");
  fprintf(stdout, ".fp 3 CB\n\\f3\n\n");
  fprintf(stdout, ".box barcode\n\\v'-1.5v'\n'ad l\n'ps 10.1c\n\\h'0.05c'\\c\n");

  //print lines
  for(int pos=0; pos<30; pos++)
    {
      if( (longtoggler%2)==1)
	fprintf(stdout, ".black %.4fc\n",  width * bc->bars[2*pos]);
      else
	fprintf(stdout, ".blacklong %.4fc\n", width * bc->bars[2*pos]);

      fprintf(stdout, ".space %.4fc\n", width * bc->bars[2*pos+1]);

      if ( 2*pos >= EAN13_LONG_SHORT[longtoggler] - 2 ) longtoggler++;
    }

  //print title .. tbd

  //print number
  fprintf(stdout, ".br\n\n\\v'2.5'");
  fputc(bc->UPC[0] +48, stdout);
  fprintf(stdout, " ");
  for (int digit=1; digit<=6; digit++)
    fputc(bc->UPC[digit] +48, stdout);
  fprintf(stdout, " ");
  for (int digit=7; digit<13; digit++)
    fputc(bc->UPC[digit] +48, stdout);
  fprintf(stdout, " >\n.sp\n.ad\n.ps\n.output\n.box\n\n.barcode\n.sp 2");

  //remove macroc
  fprintf(stdout, ".rm black\n.rm space\n.rm blacklong\n");
}
