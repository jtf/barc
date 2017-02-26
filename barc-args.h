#pragma once

struct options
{
  int quiet;
};


void parse_args(int*, char*const*, struct options*);
