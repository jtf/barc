#pragma once

struct options
{
  int quiet;
  int no_checksum;
  int debug;
  unsigned char aoc; //add-on char
};


void parse_args(int*, char*const*, struct options*);
