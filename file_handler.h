#ifndef FHANDLER
#define FHANDLER

#include "fd.h"

FILE *file_open (const char *filename);

long long int get_file_size (const char *filename);

int parse_fds_file (const char *filename, fd_set_t *fd_set);
#endif