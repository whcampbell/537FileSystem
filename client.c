#include <stdio.h>
#include "udp.h"
#include "mfs.h"

#define BUFFER_SIZE (1000)

// client code
int main(int argc, char *argv[]) {
    //struct sockaddr_in addrSnd, addrRcv;
    printf("About to init...\n");
    int rc = MFS_Init("localhost", 22346);
    printf("rc from init: %d\n", rc);
    printf("Now about to shutdown!!\n");
    rc = MFS_Shutdown();
    printf("rc from shutdown: %d\n", rc);

    // int sd = UDP_Open(20000);
    // int rc = UDP_FillSockAddr(&addrSnd, "localhost", 49321);

    // char message[BUFFER_SIZE];
    // sprintf(message, "7");

    // printf("client:: send message [%s]\n", message);
    // rc = UDP_Write(sd, &addrSnd, message, BUFFER_SIZE);
    // if (rc < 0) {
	// printf("client:: failed to send\n");
	// exit(1);
    // }

    // printf("client:: wait for reply...\n");
    // rc = UDP_Read(sd, &addrRcv, message, BUFFER_SIZE);
    // printf("client:: got reply [size:%d contents:(%s)\n", rc, message);
    return 0;
}
