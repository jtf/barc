#pragma once

struct options
{
  int quiet;
  int no_checksum;
};


void parse_args(int*, char*const*, struct options*);
