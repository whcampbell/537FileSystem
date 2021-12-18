#include <stdio.h>
#include <string.h>

#include "mfs.h"
#include "udp.h"

#define BUFFER_SIZE (1000)

int* globalPort;     // Needed for UDP Write/Read Calls
int* globalSd;       // Needed for UDP Write, Read, and Close

// Message struct used for request and replies from the server
struct message {
    // TODO, check data type and length
    char opcode[2];         // single digit opcode and null char
    char inum[5];           // max is 4095, converted to char is 4 bytes plus null char
    char pinum[5];          
    char name[BUFFER_SIZE];
    char m[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];
    char block[3];
    char type[2];   
};
// global msg
struct message *msg = NULL;

// Not sure if these are needed client side yet or not
// struct MFS_Stat_t* stat;
// struct MFS_DirEnt_t* dirEntry;
// Send and receive addrs for client/server communication
struct sockaddr_in* addrSend;
struct sockaddr_in* addrReceive;

// General Process to think about per response in piazza post @1320 
// MFS_Operation(arguments){
//      update global message struct with argument values;   
//      send_message();
// }

int MFS_Init(char *hostname, int port) {
    struct sockaddr_in* addrSnd;
    struct sockaddr_in* addrRcv;
    // dynamically allocate send and receieve addrs, as well as global ptrs
    addrSnd = malloc(sizeof(struct sockaddr_in*));
    addrRcv = malloc(sizeof(struct sockaddr_in*));
    addrSend = addrSnd;
    addrReceive = addrRcv;
    globalPort = malloc(sizeof(int*));
    globalSd = malloc(sizeof(int*));

    // open the connection to the server
    int sd = UDP_Open(port);
    if (sd <= -1) {
        return sd;
    }
    // set globals for uses in other MFS calls
    *globalSd = sd;
    *globalPort = port;
    // set up the send path to the server
    int rc = UDP_FillSockAddr(addrSend, hostname, 49337); // NOTE: This will have to vary from time to time to work on CSL Machines
    if (rc <= -1) {
        return rc;
    }
    // make sure to dynamically allocate our message struct!
    msg = (struct message*) malloc(sizeof(struct message)); 
    // should be done with init at this point
    return 0;
}


int MFS_Lookup(int pinum, char *name) {
    //update global msg struct fields
    strcpy(msg->opcode, "1");
    // make pinum a str
	char numAsStr[5];
	sprintf(numAsStr, "%d", pinum);
    strcpy(msg->pinum, numAsStr);
    strcpy(msg->name, name);

    // set the bit string packet to be sent
    char bitStr[BUFFER_SIZE];
    strcpy(bitStr, msg->opcode);
    strcat(bitStr, ",");
    strcat(bitStr, msg->pinum);
    strcat(bitStr, ",");
    strcat(bitStr, msg->name);
    char sendMsg[sizeof(struct message)];
    memcpy(sendMsg, bitStr, sizeof(struct message)); // might seem redundant right now but useful later for multiple strings (I think)
    char reply[sizeof(struct message)];

    // send packet to server
    int rc = UDP_Write(*globalSd, addrSend, sendMsg, sizeof(struct message));
    if (rc <= -1){
        printf("Failure on Client UDP Write during MFS_Lookup; rc =: %d\n", rc );
        return -1;
    }
    // wait for a reply from server
    rc = UDP_Read(*globalSd, addrReceive, reply, sizeof(struct message));
    if (rc <= -1){
        printf("Failure on Client UDP Read during MFS_Lookup; rc =: %d\n", rc );
        return -1;
    }

    int inode = -1;
    if (strcmp(reply, "-1") == 0){
        return inode;
    }
    // valid inode!
    inode = atoi(reply);
    return inode;
}


int MFS_Stat(int inum, MFS_Stat_t *m) {
    //update global msg struct fields
    strcpy(msg->opcode, "2");
    // make inum a str
	char numAsStr[5];
	sprintf(numAsStr, "%d", inum);
    strcpy(msg->inum, numAsStr);

    // set the bit string packet to be sent
    char bitStr[sizeof(struct message)];
    strcpy(bitStr, msg->opcode);
    strcat(bitStr, ",");
    strcat(bitStr, msg->inum);
    char sendMsg[sizeof(struct message)];
    memcpy(sendMsg, bitStr, sizeof(struct message)); // might seem redundant right now but useful later for multiple strings (I think)
    char reply[sizeof(struct message)];

    // send packet to server
    int rc = UDP_Write(*globalSd, addrSend, sendMsg, sizeof(struct message));
    if (rc <= -1){
        printf("Failure on Client UDP Write during MFS_Write; rc =: %d\n", rc );
        return -1;
    }
    // wait for a reply from server
    rc = UDP_Read(*globalSd, addrReceive, reply, sizeof(struct message));
    if (rc <= -1){
        printf("Failure on Client UDP Read during MFS_Write; rc =: %d\n", rc );
        return -1;
    }

    // PROCESS REPLY FROM SERVER
    //success
    if (strcmp(reply, "0")){
        return 0;
    }
    //failure
    else {
        return -1;
    }
}


int MFS_Write(int inum, char *buffer, int block) {
    //update global msg struct fields
    strcpy(msg->opcode, "3");
    // make inum a str
	char numAsStr[5];
	sprintf(numAsStr, "%d", inum);
    strcpy(msg->inum, numAsStr);
    strcpy(msg->buffer, buffer);
    char blockAsStr[3];
    sprintf(blockAsStr, "%d", block);
    strcpy(msg->block, blockAsStr);

    // set the bit string packet to be sent
    char bitStr[sizeof(struct message)];
    strcpy(bitStr, msg->opcode);
    strcat(bitStr, ",");
    strcat(bitStr, msg->inum);
    strcat(bitStr, ",");
    strcat(bitStr, msg->buffer);
    strcat(bitStr, ",");
    strcat(bitStr, msg->block);
    char sendMsg[sizeof(struct message)];
    memcpy(sendMsg, bitStr, sizeof(struct message)); // might seem redundant right now but useful later for multiple strings (I think)
    char reply[sizeof(struct message)];

    // send packet to server
    int rc = UDP_Write(*globalSd, addrSend, sendMsg, sizeof(struct message));
    if (rc <= -1){
        printf("Failure on Client UDP Write during MFS_Write; rc =: %d\n", rc );
        return -1;
    }
    // wait for a reply from server
    rc = UDP_Read(*globalSd, addrReceive, reply, sizeof(struct message));
    if (rc <= -1){
        printf("Failure on Client UDP Read during MFS_Write; rc =: %d\n", rc );
        return -1;
    }

    // PROCESS REPLY FROM SERVER
    //success
    if (strcmp(reply, "0")){
        return 0;
    }
    //failure
    else {
        return -1;
    }
}


int MFS_Read(int inum, char *buffer, int block) {
    //update global msg struct fields
    strcpy(msg->opcode, "4");
    // make inum a str
	char numAsStr[5];
	sprintf(numAsStr, "%d", inum);
    strcpy(msg->inum, numAsStr);
    strcpy(msg->buffer, buffer);
    char blockAsStr[3];
    sprintf(blockAsStr, "%d", block);
    strcpy(msg->block, blockAsStr);

    // set the bit string packet to be sent
    char bitStr[sizeof(struct message)];
    strcpy(bitStr, msg->opcode);
    strcat(bitStr, ",");
    strcat(bitStr, msg->inum);
    strcat(bitStr, ",");
    strcat(bitStr, msg->buffer);
    strcat(bitStr, ",");
    strcat(bitStr, msg->block);
    char sendMsg[sizeof(struct message)];
    memcpy(sendMsg, bitStr, sizeof(struct message)); // might seem redundant right now but useful later for multiple strings (I think)
    char reply[sizeof(struct message)];

    // send packet to server
    int rc = UDP_Write(*globalSd, addrSend, sendMsg, sizeof(struct message));
    if (rc <= -1){
        printf("Failure on Client UDP Write during MFS_Read; rc =: %d\n", rc );
        return -1;
    }
    // wait for a reply from server
    rc = UDP_Read(*globalSd, addrReceive, reply, sizeof(struct message));
    if (rc <= -1){
        printf("Failure on Client UDP Read during MFS_Read; rc =: %d\n", rc );
        return -1;
    }

    // PROCESS REPLY FROM SERVER
    //success
    if (strcmp(reply, "0")){
        return 0;
    }
    //failure
    else {
        return -1;
    }
}


int MFS_Creat(int pinum, int type, char *name) {
    //update global msg struct fields
    strcpy(msg->opcode, "5");
    // make pinum a str
	char numAsStr[5];
	sprintf(numAsStr, "%d", pinum);
    strcpy(msg->pinum, numAsStr);
    // make type a str
    char typeAsStr[2];
    sprintf(typeAsStr, "%d", type);
    strcpy(msg->type, typeAsStr);
    // update name
    strcpy(msg->name, name);

    // set the bit string packet to be sent
    char bitStr[sizeof(struct message)];
    strcpy(bitStr, msg->opcode);
    strcat(bitStr, ",");
    strcat(bitStr, msg->pinum);
    strcat(bitStr, ",");
    strcat(bitStr, msg->type);
    strcat(bitStr, ",");
    strcat(bitStr, msg->name);
    char sendMsg[sizeof(struct message)];
    memcpy(sendMsg, bitStr, sizeof(struct message)); // might seem redundant right now but useful later for multiple strings (I think)
    char reply[sizeof(struct message)];

    // send packet to server
    int rc = UDP_Write(*globalSd, addrSend, sendMsg, sizeof(struct message));
    if (rc <= -1){
        printf("Failure on Client UDP Write during MFS_CREAT; rc =: %d\n", rc );
        return -1;
    }
    // wait for a reply from server
    rc = UDP_Read(*globalSd, addrReceive, reply, sizeof(struct message));
    if (rc <= -1){
        printf("Failure on Client UDP Read during MFS_CREAT; rc =: %d\n", rc );
        return -1;
    }

    // PROCESS REPLY FROM SERVER
    //success
    if (strcmp(reply, "0")){
        return 0;
    }
    //failure
    else {
        return -1;
    }
}


int MFS_Unlink(int pinum, char *name) {
    //update global msg struct fields
    strcpy(msg->opcode, "6");
    // make pinum a str
	char numAsStr[5];
	sprintf(numAsStr, "%d", pinum);
    strcpy(msg->pinum, numAsStr);
    // update name
    strcpy(msg->name, name);

    // set the bit string packet to be sent
    char bitStr[sizeof(struct message)];
    strcpy(bitStr, msg->opcode);
    strcat(bitStr, ",");
    strcat(bitStr, msg->pinum);
    strcat(bitStr, ",");
    strcat(bitStr, msg->name);
    char sendMsg[sizeof(struct message)];
    memcpy(sendMsg, bitStr, sizeof(struct message)); // might seem redundant right now but useful later for multiple strings (I think)
    char reply[sizeof(struct message)];

    // send packet to server
    int rc = UDP_Write(*globalSd, addrSend, sendMsg, sizeof(struct message));
    if (rc <= -1){
        printf("Failure on Client UDP Write during MFS_UNLINK; rc =: %d\n", rc );
        return -1;
    }
    // wait for a reply from server
    rc = UDP_Read(*globalSd, addrReceive, reply, sizeof(struct message));
    if (rc <= -1){
        printf("Failure on Client UDP Read during MFS_UNLINK; rc =: %d\n", rc );
        return -1;
    }

    // PROCESS REPLY FROM SERVER
    //success
    if (strcmp(reply, "0")){
        return 0;
    }
    //failure
    else {
        return -1;
    }

}


int MFS_Shutdown() {
    // Send opcode 7 to server for proper shutdown
    strcpy(msg->opcode, "7"); // update msg struct (Not nec needed here)

    // set the bit string packet to be sent
    char bitStr[BUFFER_SIZE];
    strcpy(bitStr, msg->opcode);
    char sendMsg[sizeof(struct message)];
    memcpy(sendMsg, bitStr, sizeof(struct message)); // might seem redundant right now but useful later for multiple strings (I think)
    char reply[sizeof(struct message)];

    // tell server to shut down, check return code
    int rc = UDP_Write(*globalSd, addrSend, sendMsg, BUFFER_SIZE);
    if (rc <= -1){
        printf("Failure on Client UDP Write during Shutdown; rc =: %d\n", rc );
        return rc;
    }
    // wait for a reply from server that it shut down, check return code
    rc = UDP_Read(*globalSd, addrReceive, reply, BUFFER_SIZE);
    if (rc <= -1){
        printf("Failure on Client UDP Read during Shutdown; rc =: %d\n", rc );
        return rc;
    }

    //now can close connection from client to server, check return code
    rc = UDP_Close(*globalSd); 
    if (rc <= -1){
        printf("Failure on Client UDP Close during Shutdown; rc =: %d\n", rc );
        return rc;
    }
    return rc;
}