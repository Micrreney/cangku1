#ifndef __PROCESS_COPY_H__
#define __PROCESS_COPY_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

#define MOD_COPY_PATH "../mod/Copy"

int check_pram(int argc, const char *srcfile, int pronum);
int block_cur(const char *srcfile, int pronum);
int process_create(const char *srcfile, const char *destfile, int pronum, int blocksize);
void process_wait(void);

#endif
