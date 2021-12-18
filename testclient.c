#include <stdio.h>
#include "udp.h"

#define BUFFER_SIZE (1000)


// Message struct used for request and replies from the server
struct message {
    // TODO, check data type and length
    char opcode[BUFFER_SIZE];
    char pinum[BUFFER_SIZE];
    char name[BUFFER_SIZE];
    char m[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];
    char block[BUFFER_SIZE];
    char type[BUFFER_SIZE];   
};
// global msg
struct message *msg = NULL;

// client code
int main(int argc, char *argv[]) {
    struct sockaddr_in addrSnd, addrRcv;

    struct message *msg = (struct message*) malloc(sizeof(struct message)); 

    int sd = UDP_Open(22001);
    int rc = UDP_FillSockAddr(&addrSnd, "localhost", 49324);

    char message[BUFFER_SIZE];
    sprintf(message, "7");

    //todo: take the msg struct and convert into a string msg for data transfer over UDP
    strcpy(msg->opcode, "7");
    //strcpy(msg->msg, "");
    // turn struct into string and print!
    char bitStr[BUFFER_SIZE];
    strcpy(bitStr, msg->opcode);
    //strcat(bitStr,  msg->msg);
    char sendMsg[sizeof(struct message)];
    memcpy(sendMsg, bitStr, sizeof(struct message));
    char reply[BUFFER_SIZE];
    // tell server to shut down, check return code
    printf("client:: send message [%s]\n", sendMsg);
    rc = UDP_Write(sd, &addrSnd, sendMsg, BUFFER_SIZE);
    if (rc <= -1){
        return rc;
    }
    // wait for a reply from server that it shut down, check return code
    printf("client:: wait for reply...\n");
    rc = UDP_Read(sd, &addrRcv, reply, BUFFER_SIZE);
    if (rc <= -1){
        return rc;
    }
    printf("client:: got reply [size:%d contents:(%s)\n", rc, reply);

    //now can close connection from client to server, check return code
    rc = UDP_Close(sd); 
    if (rc <= -1){
        exit(1);
        //return rc;
    }

    
    return 0;
}
