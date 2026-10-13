#include "process_copy.h"
#include <unistd.h>

int block_cur(const char *srcfile, int pronum)
{
    int fd = open(srcfile, O_RDONLY);
    if(fd < 0)
    {
        perror("open src failed");
        return -1;
    }
    int fsize = lseek(fd,0, SEEK_END);
    close(fd);
    if(fsize % pronum ==0)
        return fsize / pronum;
    else
        return fsize / pronum +1;
}

