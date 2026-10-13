#include "process_copy.h"

int main(int argc, char **argv)
{
    int pronum;
    int blocksize;

    if(argv[3]==NULL)
        pronum = 3;
    else
        pronum = atoi(argv[3]);

    check_pram(argc, argv[1], pronum);
    blocksize = block_cur(argv[1],pronum);
    process_create(argv[1],argv[2],pronum, blocksize);
    return 0;
}

