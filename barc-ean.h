#pragma once

/*  A:0-3(SBSB) B:3-0(BSBS) C:0-3(BSBS)
 *  S space B bar
 *
 *  (see GS1 General Specifications 5.2.1.2.1. Symbol character
 *   encodation) */
static const unsigned char
ean_symbols[10][4] =
{
    {3, 2, 1, 1}, // 0
    {2, 2, 2, 1}, // 1
    {2, 1, 2, 2}, // 2
    {1, 4, 1, 1}, // 3
    {1, 1, 3, 2}, // 4
    {1, 2, 3, 1}, // 5
    {1, 1, 1, 4}, // 6
    {1, 3, 1, 2}, // 7
    {1, 2, 1, 3}, // 8
    {3, 1, 1, 2}  // 9
};

// mal sehen ob wir das brauchen : D
static const unsigned char
ean_symbol_direction[2][4] =
{
    //(S+B will be toggled through centre-delimiter, so A gets C)
    {0, 1, 2, 3}, // A, C
    {3, 2, 1, 0}  // B
};

// used by ean13_leftmost_order to determine pattern order
// which is used by ean_symbol_direction
enum
{
    EAN_A = 0,
    EAN_B = 1
};

// the first number is not encoded as a symbol but
// through a pattern selection
static const unsigned char
ean13_leftmost_order[10][6] =
{
    {EAN_A, EAN_A, EAN_A,   EAN_A, EAN_A, EAN_A}, // 0
    {EAN_A, EAN_A, EAN_B,   EAN_A, EAN_B, EAN_B}, // 1
    {EAN_A, EAN_A, EAN_B,   EAN_B, EAN_A, EAN_B}, // 2
    {EAN_A, EAN_A, EAN_B,   EAN_B, EAN_B, EAN_A}, // 3
    {EAN_A, EAN_B, EAN_A,   EAN_A, EAN_B, EAN_B}, // 4
    {EAN_A, EAN_B, EAN_B,   EAN_A, EAN_A, EAN_B}, // 5
    {EAN_A, EAN_B, EAN_B,   EAN_B, EAN_A, EAN_A}, // 6
    {EAN_A, EAN_B, EAN_A,   EAN_B, EAN_A, EAN_B}, // 7
    {EAN_A, EAN_B, EAN_A,   EAN_B, EAN_B, EAN_A}, // 8
    {EAN_A, EAN_B, EAN_B,   EAN_A, EAN_B, EAN_A}  // 9
};

// module count long short long short … etc
// used for how many modules have the same length in a row.
// Mainly used for emphasize guard patterns.
static const unsigned char
EAN13_LONG_SHORT[5] = {3, 27, 32, 56, 59};
//{3, 24, 5, 24, 3};

enum
{
    EAN_NORMAL_GUARD  = 0,
    EAN_CENTRE_GUARD  = 1,
    EAN_SPECIAL_GUARD = 2,
    EAN_ADD_ON_GUARD  = 3,
    EAN_ADD_ON_DELIN  = 4
};

static const unsigned char
ean_delimiter[5][7] = 
{
    // {num of S and B, patternlengths … }
    {3, 1, 1, 1, 0, 0, 0}, // normal guard
    {5, 1, 1, 1, 1, 1, 0}, // centre guard
    {6, 1, 1, 1, 1, 1, 1}, // special guard
    {3, 1, 1, 2, 0, 0, 0}, // add-on guard
    {2, 1, 1, 0, 0, 0, 0}  // add-on deliniator
};

//barc-ean.c
int parse_EAN(char**, struct barcode_data*, struct options*);

void fill_EAN_13(struct barcode_data*);
void print_EAN_13(struct barcode_data*);

