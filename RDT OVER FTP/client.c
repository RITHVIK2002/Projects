#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>

#include "rdt.h"

int main(){

    rdt_connect("127.0.0.1",8025);//ser

    int fd = open("CS3543_100MB.recv",O_RDWR| O_CREAT |O_TRUNC,(mode_t)0600);
    
    int size = 104857600;
    ftruncate(fd,size);
    void* recvbuff = mmap(NULL, size, PROT_WRITE, MAP_SHARED , fd, 0);

    rdt_recv(size, recvbuff);

    msync(recvbuff,size,MS_SYNC);
    munmap(recvbuff, size);

    
    
    
    return 0;
}
