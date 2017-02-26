#pragma once

void ignore_leading_blank(char **str);

int parse_request(char **, int, struct barcode_data *, struct options *);
int parse_barcode_type(char **, struct barcode_data *, struct options *);


