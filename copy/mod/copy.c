#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main(int argc, char ** argv)
{
    int sfd;
    int dfd;
    int offset = atoi(argv[3]);
    int blocksize = atoi(argv[4]);
    char buffer[4096];

    if((sfd = open(argv[1],O_RDONLY))==-1)
    {
        perror("open src failed");
        return -1;
    }
    if((dfd = open(argv[2],O_WRONLY|O_CREAT,0664))==-1)
    {
        perror("open dest failed");
        return -1;
    }
    lseek(sfd,offset, SEEK_SET);
    lseek(dfd,offset, SEEK_SET);

    int len;
    int want;
    printf("child %d exec copy , offset %d, blocksize %d\n",getpid(),offset,blocksize);
    while(blocksize>0)
    {
        want = blocksize < (int)sizeof(buffer) ? blocksize : (int)sizeof(buffer);
        if((len=read(sfd,buffer,want))<=0)
            break;
        write(dfd,buffer,len);
        blocksize -= len;
    }
    close(sfd);
    close(dfd);
    return 0;
}

