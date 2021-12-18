#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <fcntl.h>
#include "udp.h"
#include "mfs.h"

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

int end;
int image;
int* pieces;

//do we need a global socket descriptor?
//int sd;


int readImage(char* path) {
    image = open(path, O_RDWR);
    int* temp = malloc(sizeof(int));
    read(image, temp, sizeof(int));
    end = *temp;
    pieces = malloc(256 * sizeof(int));
    read(image, pieces, 256 * sizeof(int));

    // everyone is set up - let's go
    return 0;
}


int initImage(char* path) {

    // set up checkpoint region
    end = 257;
    pieces = malloc(256 * sizeof(int));

    // dump to file
    image = open(path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (image < 0) {
	printf("unable to create file\n");
	exit(1);
    }


    int num = end;
    write(image, &num, sizeof(num));
    write(image, pieces, 256 * sizeof(int));

    // end, pieces, and file image are all initialized - we're ready to go
    return 0;
}


// server code
int main(int argc, char *argv[]) { 
    

    // if (argc != 3) {
	// printf("usage: server <hostnum> <pathname>\n");
	// return -1;
    // }

    // if (access(argv[2], F_OK) == 0) {
	// readImage(argv[2]);
    // } else {
	// initImage(argv[2]);
    // }

    int sd = UDP_Open(49323); // NOTE: This will have to vary from time to time to work on CSL Machines
    assert(sd > -1);
    while (1) {
	struct sockaddr_in addr;
	char message[BUFFER_SIZE];
	printf("server:: waiting...\n");
	int rc = UDP_Read(sd, &addr, message, BUFFER_SIZE);

	// based on content of message, do certain operations
	//    message could be any of the eight from mfs.c
    //        *CALL*                *MESSAGE FORMAT*              *Struct fields to be made from string*
    //        lookup                ("1,pinum,name")            ; pinum->int, name->char*
	//        stat                  ("2,pinum,m")               ; pinum->int, m->MFS_Stat_t*
	//        write                 ("3,pinum,buffer,block")    ; pinum->int, buffer->char*, block->int
	//        read                  ("4,pinum,buffer,block")    ; pinum->int, buffer->char*, block->int
	//        creat                 ("5,pinum,type,name")       ; pinum->int, type->int, name->char*
	//        unlink (delete)       ("6,pinum,name")            ; pinum->int, name->char*
	//        shutdown ("7")        ("7")
	//    do this after the rc is checked (within that "if" down there)


	printf("server:: read message [size:%d contents:(%s)]\n", rc, message);
	if (rc > 0) {
            // TODO :copy the string formatted message and transform data into our msg struct
            char reply[BUFFER_SIZE];

            // SHUTDOWN Sequence
            printf("MESSAGE: %s\n", message);
            if (strcmp(message, "7") == 0 ) {
                sprintf(reply, "7 was found!!!");
                rc = UDP_Write(sd, &addr, reply, BUFFER_SIZE);
                if (rc <= -1){
                    return rc;
                }
                // close the socket for the server, check return code
                rc = UDP_Close(sd);
                if (rc <= -1)
                {
                    return rc;
                }
                else {
                    return 0;
                }
            }
            sprintf(reply, "goodbye world");
            rc = UDP_Write(sd, &addr, reply, BUFFER_SIZE);
	    printf("server:: reply\n");
	} 
    }
    return 0; 
}
