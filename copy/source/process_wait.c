#include "process_copy.h"
#include <stdio.h>
#include <errno.h>

void process_wait(void)
{
    pid_t zpid;
    while((zpid = waitpid(-1,NULL,0)) > 0 || (zpid == -1 && errno == EINTR))
    {
        if(zpid > 0)
            printf("Parent wait child success , zpid %d\n",zpid);
    }
}

