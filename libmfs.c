#include <stdio.h>
#include <string.h>

#include "mfs.h"
#include "udp.h"

#define BUFFER_SIZE (1000)

int globalPort;     // Needed for UDP Write/Read Calls
int globalSd;       // Needed for UDP Write, Read, and Close

// Message struct used for request and replies from the server
struct message {
    char opcode[BUFFER_SIZE];
    char msg[BUFFER_SIZE];
    //todo, add more fields for relevant args   
};
// global msg
struct message *msg = NULL;

// Not sure if these are needed client side yet or not
// struct MFS_Stat_t* stat;
// struct MFS_DirEnt_t* dirEntry;
// Send and receive addrs for client/server communication
struct sockaddr_in* addrSend;
struct sockaddr_in* addrRcv;

// General Process to think about per response in piazza post @1320 
// MFS_Operation(arguments){
//      update global message struct with argument values;   
//      send_message();
// }

int MFS_Init(char *hostname, int port) {
    // dynamically allocate send and receieve addrs
    addrSend = malloc(sizeof(sockaddr_in*));
    addrRcv = malloc(sizeof(sockaddr_in*));
    // open the connection to the server
    int sd = UDP_Open(port);
    if (sd <= -1) {
        return sd;
    }
    // set globals for uses in other MFS calls
    globalSd = sd;
    globalPort = port;
    // set up the send path to the server
    int rc = UDP_FillSockAddr(&addrSend, hostname, 10007); // NOTE: This will have to vary from time to time to work on CSL Machines
    if (rc <= -1) {
        return rc;
    }
    // make sure to dynamically allocate our message struct!
    struct message *msg = (struct message*) malloc(sizeof(struct message)); 
    // should be done with init at this point
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
    int rc = UDP_Write(globalPort, &addrSend, sendMsg, BUFFER_SIZE);
    if (rc <= -1){
        return rc;
    }
    // wait for a reply from server that it shut down, check return code
    rc = UDP_Read(globalPort, &addrRcv, reply, BUFFER_SIZE);
    if (rc <= -1){
        return rc;
    }
    // now can close connection from client to server, check return code
    rc = UDP_Close(globalSd); 
    if (rc <= -1){
        return rc;
    }
    return 0;
}

// to do , add a check reply, etc
