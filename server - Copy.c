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
    char opcode[2];         // single digit opcode and null char
    char inum[5];           // max is 4095, converted to char is 4 bytes plus null char
    char pinum[5];          
    char name[BUFFER_SIZE];
    char m[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];
    char block[3];
    char type[2];   
};

typedef struct inode {
    int size;
    int type;
    int pointers[14];
} INode;

// global msg
struct message *msg = NULL;

//struct sockaddr_in* addrSend;
//struct sockaddr_in* addrReceive;
struct sockaddr_in* addrGlobal = NULL;

int end;
int image;
int* pieces;

//do we need a global socket descriptor?
//int sd;

// returns location of inode
// so that you can get file offsets
int traverse(int inum) {
    int ppoint = pieces[inum % 256];
    int bucket = inum - (inum % 256);
    bucket = bucket / 256;
    lseek(image, ppoint + bucket * 4, SEEK_SET);
    int* ipoint = 0;
    read(image, ipoint, sizeof(int));
    printf("ipoint is %d\n", *ipoint);
    return *ipoint;
}


int mfRead(int inum, char* buffer, int block) {
    return -1;
}


int mfWrite(int inum, char* data) {
//get addrs of old data blocks
//write new data blocks
//write inode block with old and new data pointers
//write piece of map
//update checkpoint region with new map piece
//write checkpoint in file
//send back response
    return -1;
}


int mfCreat(char* name, int pinum, int type) {
// check if name exists in parent
// pick new inum
// add to parent directory
// send back confirm code
    return -1;
}


int mfLookup(int pinum, char* name) {      
    //int dpoint = traverse(pinum);
    // iterate through for matching name
    // send back corresponding inode
    return -1;
}


int mfStat(int inum) {
   // ipointer = traverse(inum);
//send back stat data from this inode/file
    return -1;
}


int mfUnlink(int pinum, char* name) {
 //   int dpoint = traverse(pinum);
//iterate through directory to find name
//swap its inode to -1
//send back reply
    return -1;
}


int mfShutdown(int sd, struct sockaddr_in* addr, char* reply) {
    // force to disk
    
    // send back reply
    sprintf(reply, "0");
    int rc = UDP_Write(sd, addr, reply, BUFFER_SIZE);
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
        exit(0); // success
    }
}


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
    //pieces = calloc(256 * sizeof(int));

    // dump to file
    image = open(path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (image < 0) {
	printf("unable to create file\n");
	exit(1);
    }

    //int* root = malloc(128 * sizeof(struct __MFS_DirEnt_t));
    struct __MFS_DirEnt_t self;
    struct __MFS_DirEnt_t parent;
    char* selfName = ".";
    char* parentName = "..";
    strcpy(self.name, selfName);
    strcpy(parent.name, parentName);
    //TODO finish initializing root with own inodes, inode block, map piece

    int num = end;
    write(image, &num, sizeof(num));
    write(image, pieces, 256 * sizeof(int));

    // end, pieces, and file image are all initialized - we're ready to go
    return 0;
}





















// server code
int main(int argc, char *argv[]) { 
    

    if (argc != 3) {
	printf("usage: server <hostnum> <pathname>\n");
	return -1;
    }

    if (access(argv[2], F_OK) == 0) {
	readImage(argv[2]);
    } else {
	initImage(argv[2]);
    }

    int sd = UDP_Open(49334); // NOTE: This will have to vary from time to time to work on CSL Machines
    assert(sd > -1);
    while (1) {
	struct sockaddr_in* addr = malloc(sizeof(struct sockaddr_in*));
    // dynamically allocate send and receieve addrs, as well as global ptrs
    addrGlobal = malloc(sizeof(struct sockaddr_in*));
    addrGlobal = addr;
	char message[BUFFER_SIZE];
	printf("server:: waiting...\n");
	int rc = UDP_Read(sd, addr, message, BUFFER_SIZE);

	//        *CALL*                *MESSAGE FORMAT*              *Struct fields to be made from string*
    //        lookup                ("1,pinum,name")            ; pinum->int, name->char*
	//        stat                  ("2,pinum,m")               ; pinum->int, m->MFS_Stat_t*
	//        write                 ("3,inum,buffer,block")     ; inum->int, buffer->char*, block->int
	//        read                  ("4,inum,buffer,block")     ; inum->int, buffer->char*, block->int
	//        creat                 ("5,pinum,type,name")       ; pinum->int, type->int, name->char*
	//        unlink (delete)       ("6,pinum,name")            ; pinum->int, name->char*
	//        shutdown ("7")        ("7")
	//    do this after the rc is checked (within that "if" down there)
	


	printf("server:: read message [size:%d contents:(%s)]\n", rc, message);
	if (rc > 0) {
            
            char reply[BUFFER_SIZE];
            int opcode = atoi((void*) message[0];
            switch(opcode) { 
                case 1: 
                    break;
                case 2: 
                    break;
                case 3: 
                    break;
                case 4: 
                    break;
                case 5: 
                    break;
                case 6: 
                    break;
                case 7:
                    mfShutdown(sd, addrGlobal, reply);
                    break;
            }
            

            // // SHUTDOWN Sequence TODO: MOVE THIS TO HELPER METHOD
            // if (strcmp(message, "7") == 0 ) {
            //     sprintf(reply, "0");
            //     rc = UDP_Write(sd, &addr, reply, BUFFER_SIZE);
            //     if (rc <= -1){
            //         return rc;
            //     }
            //     // close the socket for the server, check return code
            //     rc = UDP_Close(sd);
            //     if (rc <= -1)
            //     {
            //         return rc;
            //     }
            //     else {
            //         exit(0); // success
            //     }
            // }
            sprintf(reply, "goodbye world");
            rc = UDP_Write(sd, addr, reply, BUFFER_SIZE);
	    printf("server:: reply\n");
	} 
    }
    return 0; 
}
