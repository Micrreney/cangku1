#include "process_copy.h"
#include <stdio.h>
#include <stdlib.h>

int check_pram(int argc, const char *srcfile, int pronum)
{
    if(argc < 3)
    {
        printf("Usage: ./main src dest [pronum]\n");
        exit(EXIT_FAILURE);
    }
    if(pronum <=0)
    {
        printf("process num must >0\n");
        exit(EXIT_FAILURE);
    }
    struct stat st;
    if(stat(srcfile,&st) <0)
    {
        perror("src file");
        exit(EXIT_FAILURE);
    }
    if(!S_ISREG(st.st_mode))
    {
        printf("src is not regular file\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}

