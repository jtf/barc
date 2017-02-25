#pragma once

//barc-ean.c
int parse_ISBN(char**, struct barcode_data *);

void fill_EAN_13(struct barcode_data *);
void print_EAN_13(struct barcode_data *);

