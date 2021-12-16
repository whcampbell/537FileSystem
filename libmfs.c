#include <stdio.h>
#include <string.h>

#include "mfs.h"
#include "udp.h"

#define BUFFER_SIZE (1000)

int globalPort;
int globalSd;

int MFS_Init(char *hostname, int port) {
    globalPort = port;
    struct sockaddr_in addrSnd;
    int sd = UDP_Open(port);
    globalSd = sd;
    int rc = UDP_FillSockAddr(&addrSnd, hostname, port);
    if (sd == -1){
        return sd;
    }
    if (rc == -1){
        return sd;
    }
    return 0;
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
    char reply[BUFFER_SIZE];
    struct addrRcv;
    int rcRead = UDP_Read(globalPort, &addrRcv, reply, BUFFER_SIZE);
    int rcClose = UDP_Close(globalSd); 
    if (rcRead | rcClose){
        return -1;
    }
    return 0;
}

// to do , add a check reply, etc
