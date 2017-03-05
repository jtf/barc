#include <stdio.h>
#include <ctype.h>    // for isdigit()
//#include <string.h>   // for strlen()
#include "barc.h"
#include "barc-ean.h"



void debug_print_type(struct barcode_data *bc, struct options *o)
{
    int type;
    type = bc->barcode_type;

    if(! o->debug || o->quiet) return;
    fprintf(stderr, "----> [");
    switch(type)
    {
	// vague and unknown types
    case undefined   : fprintf(stderr, "undefined"); break;
    case ISBNx       : fprintf(stderr, "ISBNx"); break;
    case EANx        : fprintf(stderr, "EANx"); break;
    case ISBN_10x    : fprintf(stderr, "ISBN_10x"); break;
    case ISBN_13x    : fprintf(stderr, "ISBN_13x"); break;
    case EAN_13x     : fprintf(stderr, "EAN_13x"); break;
	// addon error codes
    case ISBNx_addon : fprintf(stderr, "ISBNx_addon"); break;
    case EANx_addon  : fprintf(stderr, "EANx_addon"); break;
	// known barcodes
    case EAN_8       : fprintf(stderr, "EAN_8"); break;
    case EAN_13      : fprintf(stderr, "EAN_13"); break;
    case ISBN_10     : fprintf(stderr, "ISBN_10"); break;
    case ISBN_13     : fprintf(stderr, "ISBN_13"); break;
    case UPC_A       : fprintf(stderr, "UPC_A"); break;
    case UPC_B       : fprintf(stderr, "UPC_B"); break;
    case UPC_C       : fprintf(stderr, "UPC_C"); break;
    case UPC_D       : fprintf(stderr, "UPC_D"); break;
    case UPC_E       : fprintf(stderr, "UPC_E"); break;
    
	// barcodes with additional barcode : )
    case EAN_13_2_addon  : fprintf(stderr, "EAN_13_2_addon"); break;
    case EAN_13_5_addon  : fprintf(stderr, "EAN_13_5_addon"); break;
    case ISBN_10_2_addon : fprintf(stderr, "ISBN_10_2_addon"); break;
    case ISBN_10_5_addon : fprintf(stderr, "ISBN_10_5_addon"); break;
    case ISBN_13_2_addon : fprintf(stderr, "ISBN_13_2_addon"); break;
    case ISBN_13_5_addon : fprintf(stderr, "ISBN_13_5_addon"); break;
    case UPC_A_2_addon   : fprintf(stderr, "UPC_A_2_addon"); break;
    case UPC_A_5_addon   : fprintf(stderr, "UPC_A_5_addon"); break;
    case UPC_E_2_addon   : fprintf(stderr, "UPC_E_2_addon"); break;
    case UPC_E_5_addon   : fprintf(stderr, "UPC_E_5_addon"); break;
    }
    fprintf(stderr, "] <----\n");
}

// calculate checksum depending on barcode type
// return 0 if checksum differs from input else 1
int
calc_ean_checksum(struct barcode_data *bc)
{
    int rv = 0; //return value
    bc->checksum = 0;  // init checksum
    switch(bc->barcode_type)
    {
    case ISBN_10:
    case ISBN_10_2_addon:
    case ISBN_10_5_addon:
	for (int i=0; i<9; i++)
	{
	    bc->checksum += bc->UPC[i]*(i+1);
	}
	bc->checksum %= 11;
	rv = (bc->UPC[9] == bc->checksum);
	break;
    case EAN_13:
    case EAN_13_2_addon:
    case EAN_13_5_addon:
    case ISBN_13:
    case ISBN_13_2_addon:
    case ISBN_13_5_addon:
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
    if (bc->barcode_type != ISBN_10
	&& bc->barcode_type != ISBN_10_2_addon
	&& bc->barcode_type != ISBN_10_5_addon ) return;

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

int
parse_EAN(char **ean, struct barcode_data *bc, struct options *o)
{
    char * digit_p;
    unsigned char * title_p;
    
    digit_p = *ean;
    title_p = bc->title;

    int pos = 0;
    unsigned char *UPC;
    // set UPC first on main code (later on add-on code if needed)
    UPC = bc->UPC;

    debug_print_type(bc, o);

    // debug output title/which sort of barcode type
    if (!o->quiet)
    {
	switch(bc->barcode_type)
	{
	case ISBNx: fprintf(stderr, "║│ ║││║   ISBN: "); break;
	case EANx:  fprintf(stderr, "║│ ║││║   EAN:  "); break;
//	    case ISSNx: fprintf(stderr, "║│ ║││║   ISSN: "); break;
//	    case ISMNx: fprintf(stderr, "║│ ║││║   ISMN: "); break;
	}
    }

    while(*digit_p != '\n')
    {

	// check for last ISBN-10 checksum X
	// special treatment
	if(bc->barcode_type==ISBNx
	   && (*digit_p=='X' || *digit_p=='x')
	   && pos==10 )
	{
	    if (!o->quiet) fprintf(stderr, "%c (X found) ", *digit_p);
	    UPC[pos] = 9; // 10th
	    pos++;
	}
	// found 13th number of ISBN or EAN .. so we have ISBN-13 or EAN-13
	else if((bc->barcode_type==ISBNx || bc->barcode_type==EANx)
		&& isdigit(*digit_p)
		&& pos == 12) //13th
	{
	    if (!o->quiet) fprintf(stderr, "%c ", *digit_p);
	    UPC[pos++] = *digit_p - 48;
	    switch(bc->barcode_type)
	    {
	    case ISBNx: bc->barcode_type=ISBN_13;break;
	    case EANx: bc->barcode_type=EAN_13;break;
	    }
	}
	
	// found separation character for ISBN-10
	else if((bc->barcode_type==ISBNx || bc->barcode_type==ISBN_10)
		&& *digit_p == o->aoc
		&& pos == 10) //10th +1
	{
	    if (!o->quiet) fprintf(stderr, "\n║│ ║││║   found addon: \"%c\" ", *digit_p);
	    pos=0;
	    UPC=bc->addon;
	    bc->barcode_type=ISBN_10x;
	}
	// found separation character for ISBN 13
	else if((bc->barcode_type==ISBN_13 || bc->barcode_type==EAN_13)
		&& *digit_p == o->aoc
		&& pos == 13) //13th +1
	{
	    if (!o->quiet) fprintf(stderr, "\n║│ ║││║   found addon: \"%c\" ", *digit_p);
	    pos=0;
	    UPC=bc->addon;
	    switch(bc->barcode_type)
	    {
	    case ISBN_13: bc->barcode_type=ISBN_13x;break;
	    case EAN_13:  bc->barcode_type=EAN_13x;
	    }
	}
	// addon to long
	else if( (bc->barcode_type==ISBN_10x
		  || bc->barcode_type==ISBN_13x
		  || bc->barcode_type==EAN_13x)
		 && pos > 5)
	    break;
	else if (isdigit(*digit_p))
	{
	    if (!o->quiet) fprintf(stderr, "%c ", *digit_p);
	    UPC[pos++] = *digit_p - 48;
	}
	digit_p++;
    }
    if (!o->quiet) fprintf(stderr, "\n");
    *title_p = '\0';

    debug_print_type(bc, o);
    
    // ok we have the line scanned, now set rest of types
    // set addon type
    if ((pos == 2 || pos ==5)
	&& (bc->barcode_type==ISBN_10x
	    || bc->barcode_type==ISBN_13x
	    || bc->barcode_type==EAN_13x))
    {
	if(pos==2)
	    switch(bc->barcode_type)
	    {
	    case ISBN_10x: bc->barcode_type=ISBN_10_2_addon;break;
	    case ISBN_13x: bc->barcode_type=ISBN_13_2_addon;break;
	    case EAN_13x:  bc->barcode_type=EAN_13_2_addon;break;
	    }
	else switch(bc->barcode_type)
	     {
	     case ISBN_10x: bc->barcode_type=ISBN_10_5_addon;break;
	     case ISBN_13x: bc->barcode_type=ISBN_13_5_addon;break;
	     case EAN_13x:  bc->barcode_type=EAN_13_5_addon;break;
	     }
    }
    // set broken addon type
    else if ((pos < 5) && (pos > 2)
	     && (bc->barcode_type==ISBN_10x
		 || bc->barcode_type==ISBN_13x
		 || bc->barcode_type==EAN_13x))
    {
	switch(bc->barcode_type)
	{
	case ISBN_10x: 
	case ISBN_13x: bc->barcode_type=ISBNx_addon;break;
	case EAN_13x:  bc->barcode_type=EANx_addon;break;
	}
    }
    // when addon is zero length ignore it
    else if (pos == 0
	     && (bc->barcode_type==ISBN_10x
		 || bc->barcode_type==ISBN_13x
		 || bc->barcode_type==EAN_13x))
    {
	if (!o->quiet) fprintf(stderr, "║│!║││║   separator but no addon given (addon ignored)\n");
	switch(bc->barcode_type)
	{
	case ISBN_10x: bc->barcode_type=ISBN_10;break;
	case ISBN_13x: bc->barcode_type=ISBN_13;break;
	case EAN_13x:  bc->barcode_type=EAN_13;
	}
    }
    // set isbn-10 type
    else if ((pos == 10)
	     && bc->barcode_type==ISBNx)
    {
	bc->barcode_type=ISBN_10;//；

    }
    else if (!(bc->barcode_type > 0))
	bc->barcode_type=undefined;

    debug_print_type(bc, o);
    
    // do some post formating for different codes
    switch(bc->barcode_type)
    {
	// ------------------- ISBN 10 -------------------------
    case ISBN_10 :
    case ISBN_10_2_addon :
    case ISBN_10_5_addon :
	if (!o->quiet)
	{
	    fprintf(stderr, "║│ ║││║   found ISBN-10\n║│ ║││║   1-14: ");
	    for(int i=0; i<14; i++)
		if (!o->quiet) fprintf(stderr, "%d ", bc->UPC[i]);
	    if (!o->quiet) fprintf(stderr, "\n");
	}

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
		for(int i=0; i<14; i++) fprintf(stderr, "%d ", bc->UPC[i]);
		fprintf(stderr, "\n");
		fprintf(stderr, "║│ ║││║        new calculated checksum: %d\n", bc->checksum);
	    }
	}
	else
	{
	    if (!o->quiet)
		fprintf(stderr, "║│!║││║   ISBN-10 checksum error  ^ (should be %d, is %d)\n",
			bc->checksum, bc->UPC[9]);
	    return o->no_checksum; // if set return 1 else 0
	}
	break;
	// ------------------- ISBN 13 and EAN 13-------------------------
    case ISBN_13 :
    case ISBN_13_2_addon :
    case ISBN_13_5_addon :
    case EAN_13 :
    case EAN_13_2_addon :
    case EAN_13_5_addon :
	if (!o->quiet)
	{
	    switch(bc->barcode_type)
	    {
	    case ISBN_13: fprintf(stderr, "║│ ║││║   found ISBN-13\n║│ ║││║   1-14: ");break;
	    case EAN_13:  fprintf(stderr, "║│ ║││║   found EAN-13\n║│ ║││║   1-14: ");break;
	    }
	    for(int i=0; i<14; i++)
		fprintf(stderr, "%d ", bc->UPC[i]);
	    fprintf(stderr, "\n");
	}
	if(calc_ean_checksum(bc))
	{
	    if (!o->quiet) fprintf(stderr, "║│ ║││║            calculated checksum: %d\n", bc->checksum);
	}
	else
	{
	    if (!o->quiet)
	    {
		fprintf(stderr, "║│!║││║           checksum error        ^ (should be %d, is %d)\n",
			bc->checksum, bc->UPC[12]);
		if (o->no_checksum) fprintf(stderr, "║│!║││║   Warning: Faulty barcode will be generated!\n");
	    }
	    return o->no_checksum; // if set return 1 else 0
	}
	break;
	// ------------------- ISBN too short error -------------------------
    case ISBNx:
	if (!o->quiet) fprintf(stderr, "║│!║││║   Error parsing ISBN (too short?)\n");
	return 0;
	// ------------------- unknown barcode -------------------------
    default :
	if (!o->quiet) fprintf(stderr, "║│!║││║   Error parsing barcode, not implemented yet, who knows?\n");
	return 0;
    }
    return 1;
}

// —— FILL BARCODE WITH BARS —————————————————————————————————————————————————

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
	 [ean13_leftmost_order[bc->UPC[0]][digit]]
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
 *  insert n digits in given numberset
 * pos    -- bar/space-position
 * num    -- number off symbols to encode
 * offset -- UPC position to start from
 * set    -- how the symbols should be encoded EAN_A/B/C
 * bc     -- barcode struct to work on
 */
int
insert_n_digits(int pos, int num, int offset, int set, struct barcode_data *bc)
{
    for (int i=0; i<offset; i++)
	for (int modul=0; modul<4; modul++)
	{
		bc->bars[pos++] =
		ean_symbols[bc->UPC[num+i]]
		[ean_symbol_direction[set][modul]];
	}
    return pos;
}

/*
 *  take barcode struct and fill bars with bar lengths for EAN-13
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
}

/*
 *  take barcode struct and fill bars with bar lengths for EAN-8
 */
void
fill_EAN_8(struct barcode_data *bc)
{
  int pos = 0;
  pos = insert_guard(pos, EAN_NORMAL_GUARD, bc);
  pos = insert_n_digits(pos, 4, 0, EAN_A, bc);
  pos = insert_guard(pos, EAN_CENTRE_GUARD, bc);
  pos = insert_n_digits(pos, 4, 4, EAN_C, bc);
  pos = insert_guard(pos, EAN_NORMAL_GUARD, bc);
}

/*
 *  take barcode struct and fill bars with bar lengths for UPC-A
 * why not use ean-13 with leading zero??
 */
void
fill_UPC_A(struct barcode_data *bc)
{
  int pos = 0;
  pos = insert_guard(pos, EAN_NORMAL_GUARD, bc);
  pos = insert_n_digits(pos, 6, 1, EAN_A, bc);
  pos = insert_guard(pos, EAN_CENTRE_GUARD, bc);
  pos = insert_n_digits(pos, 6, 7, EAN_C, bc);
  pos = insert_guard(pos, EAN_NORMAL_GUARD, bc);
}

/*
 *  take barcode struct and fill bars with bar lengths for UPC-E
 */

/* void */
/* fill_UPC_E(struct barcode_data *bc) */
/* { */
/*   int pos = 0; */
/*   pos = insert_guard(pos, EAN_NORMAL_GUARD, bc); */
/*   pos = FUCKUP */
/*   pos = insert_guard(pos, EAN_SPECIAL_GUARD, bc); */
/* } */


void
fill_addon_2(struct barcode_data *bc)
{
    int check = (bc->addon[0] * 10 + bc->addon[1]) % 4;
    int pos = 0;

    pos = insert_guard(pos, EAN_ADD_ON_GUARD, bc);
    for (int i=0; i<=1; i++)
	for (int modul=0; modul<4; modul++)
	{
	    //                                                A or B
	    bc->addbars[pos++] =
		ean_symbols
		[ean_symbol_direction[bc->addon[i]][ean13_leftmost_order[check][i+3]]]
		[modul];
	    if(i==0)
		pos = insert_guard(pos, EAN_ADD_ON_DELIM, bc);
	}

  /* for (int digit=0; digit<6; digit++) */
  /*   for (int modul=0; modul<4; modul++) */
  /*     bc->bars[pos++] = */
  /* 	ean_symbols[bc->UPC[digit+7]] */
  /* 	[ean_symbol_direction[EAN_A][modul]]; */

}

/* void */
/* fill_addon_5(struct barcode_data *bc) */
/* { */
/* } */


// —— DO TROFF OUTPUT ————————————————————————————————————————————————————————

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
  fprintf(stdout, ".box barcode_out\n\\v'-1.5v'\n'ad l\n'ps 10.1c\n\\h'0.05c'\\c\n");
                       // changed boxname from barcode to barcode_out
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
  fprintf(stdout, " >\n.sp\n.ad\n.ps\n.output\n.box\n\n.barcode_out\n.sp 2");

  //remove macroc
  fprintf(stdout, ".rm black\n.rm space\n.rm blacklong\n");
}
