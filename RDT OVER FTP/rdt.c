#include <stdio.h>                      
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>

#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <pthread.h>

#include <sys/mman.h>
#include <sys/time.h>

/**
 * PKT --- SEQ_NO(4bytes) + PAYLOAD(max.4092bytes)
 */
#define PKT_SIZE 8096
#define PAYLOAD_SIZE 8092


/**
 * Globals: 
 *  
 * 
 */

int MSG_LEN, TOTAL_PKTS, pkts_sent, pkts_recv;
char* MSG;

int small_pkt_size; /*If the payload is not mutiple of payload_size*/

int soc_fd;
struct sockaddr_in addr_other;

//pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_t worker_thread;

/**********************************************************************/

void rdt_bind(char* ip, int port)
{
    soc_fd = socket(AF_INET, SOCK_DGRAM, 0);
    struct sockaddr_in addr_self;

    addr_self.sin_family = AF_INET;
    inet_aton(ip, &addr_self.sin_addr);
    addr_self.sin_port = htons(port);

    bind(soc_fd, (struct sockaddr *)&addr_self, sizeof(addr_self));
}

void rdt_connect(char* ip, int port)
{
    soc_fd = socket(AF_INET, SOCK_DGRAM, 0);

    addr_other.sin_family = AF_INET;
    inet_aton(ip, &addr_other.sin_addr);
    addr_other.sin_port = htons(port);

    
    char buff;
    sendto(soc_fd, &buff, sizeof(buff), 0, (struct sockaddr* )&addr_other,sizeof(addr_other));
    
    //Make it the default dest addresses 
    connect(soc_fd,(struct sockaddr* )&addr_other,sizeof(addr_other));
}

void rdt_listen()
{
    char buff;
    int addr_len = sizeof(addr_other);
    recvfrom(soc_fd, &buff, sizeof(buff), 0, (struct sockaddr* )&addr_other, &addr_len);

    //Make it the default dest addresses 
    connect(soc_fd,(struct sockaddr* )&addr_other,sizeof(addr_other));

}

// Helper Function to send a packet given a seqno
void rdt_send_pkt_with_seq_no(int seq_no,char* pkt_buff)
{
    *((int*)pkt_buff) = seq_no ; //Seq_no

    
    if(small_pkt_size && seq_no==TOTAL_PKTS-1)
        memcpy(pkt_buff+sizeof(int) , &MSG[seq_no*PAYLOAD_SIZE], small_pkt_size);
    else
        memcpy(pkt_buff+sizeof(int) , &MSG[seq_no*PAYLOAD_SIZE], PAYLOAD_SIZE);

    // Now send the pkt
    int a = send(soc_fd, pkt_buff, PKT_SIZE, 0);

    //printf("Sending pkt: %d/%d\t%d\n",seq_no,TOTAL_PKTS,a);
}

// Thread function for listening the req for retrasmission
void* rdt_sender_thread(void * arg)
{
    int seq_no;
    char* pkt_buff = (char*) malloc(PKT_SIZE);

    while(1)
    {
        recv(soc_fd, &seq_no, sizeof(seq_no), 0);
        if(seq_no < 0)
            return NULL;
        rdt_send_pkt_with_seq_no(seq_no,pkt_buff);
    }
    
}

void rdt_send(int len, char* buff)
{
    MSG_LEN = len;
    TOTAL_PKTS = ceil(MSG_LEN / (float)PAYLOAD_SIZE);
    
    small_pkt_size = MSG_LEN%PAYLOAD_SIZE;
    MSG = buff;

    pkts_sent =0;
    
    char* pkt_buff = (char*) malloc(PKT_SIZE);

    for (int i = 0; i < TOTAL_PKTS; i++)
    {
        if(i%120==0)
            usleep(80000);
        rdt_send_pkt_with_seq_no(i, pkt_buff);
    }
       
    
    
    // Start a thread for listening the req for retrasmission
    pthread_create(&worker_thread,0,&rdt_sender_thread,NULL);

    pthread_join(worker_thread,NULL);
    

}



/******************************************************************/

void* rdt_recv_thread(void* arg)
{   
    int* pkts_mark = (int*) arg;
    while (pkts_recv < TOTAL_PKTS)
    {    
        for (int i = 0; i < TOTAL_PKTS ; i++)
        {
            if(!pkts_mark[i])
            {
                if(i%300==0)
                    usleep(50000);
                send(soc_fd, &i , sizeof(i), 0);
                //printf("%d:%d\t ",pkts_mark[i]);
            }
        }
        // Wait some time before trying again
        usleep(50000);  
    }
}


void rdt_recv(int len, char* buff)
{
    MSG_LEN = len;
    TOTAL_PKTS = ceil(MSG_LEN / (float)PAYLOAD_SIZE);
    small_pkt_size = MSG_LEN%PAYLOAD_SIZE;

    pkts_recv =0;

    int pkts_mark[TOTAL_PKTS];
    memset(pkts_mark,0,sizeof(int)*TOTAL_PKTS);

    bool thread_started;

    int seq_no;
    char* pkt_buff = (char*) malloc(PKT_SIZE);
    
    struct timeval timeoutval = {.tv_usec = 400000}, start_time, end_time; //in s
    //  Time out interval
    setsockopt(soc_fd,SOL_SOCKET, SO_RCVTIMEO, &timeoutval , sizeof(timeoutval) );

    gettimeofday(&start_time,NULL);

    while(pkts_recv < TOTAL_PKTS)
    {
        int bytes_recv = recv(soc_fd, pkt_buff, PKT_SIZE, 0);
        
        if(!thread_started && bytes_recv <= 0)
        {
            thread_started = true;
            pthread_create(&worker_thread, 0, &rdt_recv_thread, pkts_mark);
            //printf("Thread Started: %d/%d\n",pkts_recv,TOTAL_PKTS);
            continue;
        }
        
        seq_no = *(int*)pkt_buff;
        if(pkts_mark[seq_no])
            continue;
        
        if(small_pkt_size && seq_no==TOTAL_PKTS-1)
            memcpy(&buff[seq_no*PAYLOAD_SIZE], pkt_buff+sizeof(int), small_pkt_size);
        else
            memcpy(&buff[seq_no*PAYLOAD_SIZE], pkt_buff+sizeof(int), PAYLOAD_SIZE);

        __atomic_fetch_add(&pkts_recv, 1, __ATOMIC_SEQ_CST);
        __atomic_fetch_add(&pkts_mark[seq_no],  1, __ATOMIC_SEQ_CST);      

        //printf("Seq: %d\t%d/%d\t%d\n",seq_no,pkts_recv,TOTAL_PKTS,bytes_recv) ; 

    }
    gettimeofday(&end_time,NULL);

    printf("Elapsed time: %lfms\n",((end_time.tv_sec-start_time.tv_sec)*1000)+((end_time.tv_usec-start_time.tv_usec)/1000.0));

    pthread_detach(worker_thread);

    //Send ack as the packet recieved
    for (int i = 0; i < 5; i++)
    {
        int seq_no = -1;
        send(soc_fd, &seq_no , sizeof(seq_no), 0);
    }
    

}