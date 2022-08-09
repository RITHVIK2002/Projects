#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>


#include "rdt.h"

int main(){

    
    rdt_bind("192.168.122.80",8025); //ser
    rdt_listen();

    //Open a file for read-only
    int fd = open("CS3543_100MB",O_RDONLY);

    struct stat stat_buf;
    int rc = fstat(fd, &stat_buf);
    int size =  rc == 0 ? stat_buf.st_size : -1;


    void* sendbuff = mmap(NULL, size, PROT_READ, MAP_PRIVATE , fd, 0);

    rdt_send(size,sendbuff);

    munmap(sendbuff, size);
    
    
    
    return 0;
}