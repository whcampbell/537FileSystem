#include <stdio.h>
#include <string.h>

#include "mfs.h"
#include "udp.h"

int globalPort;

int MFS_Init(char *hostname, int port) {
    globalPort = port;
   // struct sockaddr_in addr;
   // int rc_open = UDP_Open(port);
   // int rc_socket = UDP_FillSockAddr(&addr, hostname, port);
   // if (rc_open | rc_socket){
   //     return -1;
   // }
    return 0;
    //printf("Not very fancy MFS_Init %s %d\n", hostname, port);
    //return 0;
}


int MFS_Lookup(int pinum, char *name) {
    return 0;
}


int MFS_Stat(int inum, MFS_Stat_t *m) {
    return 0;
}


int MFS_Write(int inum, char *buffer, int block) {
    return 0;
}


int MFS_Read(int inum, char *buffer, int block) {
    return 0;
}


int MFS_Creat(int pinum, int type, char *name) {
    return 0;
}


int MFS_Unlink(int pinum, char *name) {
    return 0;
}


int MFS_Shutdown() {
    return UDP_Close(globalPort); // TODO, get the fd
}