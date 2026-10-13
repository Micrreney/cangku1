#include "process_copy.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

int process_create(const char * srcfile, const char * destfile, int pronum , int blocksize)
{
    char offset_str[32];
    char blocksize_str[32];
    int dfd;

    dfd = open(destfile, O_WRONLY|O_CREAT|O_TRUNC, 0664);
    if(dfd < 0)
    {
        perror("open dest failed");
        return -1;
    }
    close(dfd);

    pid_t pid;
    int i;
    for(i = 0 ; i < pronum; i++)
    {
        pid = fork();
        if(pid == 0)
        {
            break;
        }
    }

    if(pid > 0)
    {
        process_wait();
    }
    else if(pid == 0)
    {
        int offset;
        offset = i * blocksize;
        sprintf(offset_str, "%d" , offset);
        sprintf(blocksize_str, "%d" , blocksize);
        printf("child %d exec Copy , offset %d, blocksize %d\n",getpid(),offset,blocksize);
        execl(MOD_COPY_PATH,"Copy",srcfile,destfile , offset_str,blocksize_str,NULL);
        perror("execl");
    }
    else
    {
        perror("fork");
        return -1;
    }
    return 0;
}
