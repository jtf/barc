#pragma once

#include "barc-args.h"

// define type of barcode
enum bc_type
{
    // vague and unknown types
    undefined   = -1,
    ISBNx       = -2,
    EANx        = -3,
    ISBN_10x    = -4,
    ISBN_13x    = -5,
    EAN_13x     = -6,
    // addon error codes
    ISBNx_addon = -7,
    EANx_addon   = -8,

    // known barcodes
    EAN_8 = 1,
    EAN_13,
    ISBN_10,
    ISBN_13,
    UPC_A,
    UPC_B,
    UPC_C,
    UPC_D,
    UPC_E,
    
    // barcodes with additional barcode : )
    EAN_13_2_addon,
    EAN_13_5_addon,
    ISBN_10_2_addon,
    ISBN_10_5_addon,
    ISBN_13_2_addon,
    ISBN_13_5_addon,
    UPC_A_2_addon,
    UPC_A_5_addon,
    UPC_E_2_addon,
    UPC_E_5_addon
};

struct
barcode_data
{
  int checksum;
  int barcode_type;
  unsigned char UPC[14];
  unsigned char addon[5];
  unsigned char title[40];

  // 59 stripes -> save 59 lengths  ( 3 + 6*(4) + 5 + 6*(4) + 3 = 59 )
  // for EAN-13, ISBN-13 and UPC-A
  unsigned char bars[59];
};// bc_data;
