#pragma once

#include "barc-args.h"

// define type of barcode
enum bc_type
{
    undefined = -1,
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
    EAN_13_addon,
    ISBN_10_addon,
    ISBN_13_addon,
    UPC_A_addon,
    UPC_E_addon
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
