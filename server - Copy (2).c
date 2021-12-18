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

char replyGlobal[BUFFER_SIZE];

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

int mfLookup(int pinum, char* name) {      
    //int dpoint = traverse(pinum);
    // iterate through for matching name
    // send back corresponding inode
    return -1;
}


int mfRead(int inum, char* buffer, int block) {
    return -1;
}


int mfWrite(int inum, char* buffer, int block) {
//get addrs of old data blocks
//write new data blocks
//write inode block with old and new data pointers
//write piece of map
//update checkpoint region with new map piece
//write checkpoint in file
//send back response
    return -1;
}


int mfCreat(int pinum, int type, char* name) {
    // check if name exists in parent
    int exists = mfLookup(pinum, name);
    if (exists) {
        //TODO send back success
        return 0;
    }
    // pick new inum
    int i;
    int prevPiece = pieces[0];
    int mapPiece[16];
    int inum = -1;
    for (i = 1; i < 256; ++i) { 
        if (pieces[i] == 0) { // we're entering the land of empty pieces
            lseek(image, prevPiece, SEEK_SET); // check if the last piece is entirely full
            read(image, mapPiece, sizeof(int) * 16);
            int j;
            for (j = 0; j < 16; ++j) {
            if (mapPiece[j] == 0) {
            inum = j * 256;
            inum += i; // i is one too big, but inum starts at -1, so it all good
            --i;
            } else {
                continue;
            }
            }
            if (inum == -1) { // an empty inum was not found in the prev map piece
            inum = i; // put it in the first spot of the next map piece
            }	
        
        } else {
            prevPiece = pieces[i];
            continue;
        }
    } 
    
    // go to end of file
    lseek(image, end, SEEK_SET);
    // if directory, add directory data block
    // add inode to end of list with map piece
    struct inode newNode;
    newNode.size = 0;
    newNode.type = type;
    
    // add to checkpoint region - update in mem and on disk
    // add to parent directory
    // send back confirm code
    return 0;
}


int mfLookup(int pinum, char* name) {      
    int dpoint = traverse(pinum);
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

    int sd = UDP_Open(49337); // NOTE: This will have to vary from time to time to work on CSL Machines
    assert(sd > -1);
    while (1) {
	struct sockaddr_in addr;
	char message[BUFFER_SIZE];
	printf("server:: waiting...\n");
	int rc = UDP_Read(sd, &addr, message, BUFFER_SIZE);

	//        *CALL*                *MESSAGE FORMAT*              *Struct fields to be made from string*
    //        lookup                ("1,pinum,name")            ; pinum->int, name->char*
	//        stat                  ("2,pinum)                  ; pinum->int
	//        write                 ("3,inum,buffer,block")     ; inum->int, buffer->char*, block->int
	//        read                  ("4,inum,buffer,block")     ; inum->int, buffer->char*, block->int
	//        creat                 ("5,pinum,type,name")       ; pinum->int, type->int, name->char*
	//        unlink (delete)       ("6,pinum,name")            ; pinum->int, name->char*
	//        shutdown ("7")        ("7")
	//    do this after the rc is checked (within that "if" down there)
	


	printf("server:: read message [size:%d contents:(%s)]\n", rc, message);
    

	if (rc > 0) {

        char reply[BUFFER_SIZE];
        int shutdown = -1;

        // parse message
        char* messageParse = strdup(message);
        char* opcode;
        if ((opcode = strsep(&messageParse, ",")) != NULL) {

            /********* LOOKUP **********/
            if (strcmp(opcode, "1") == 0){
                char* pinum = strsep(&messageParse, ",");
                char* name = strsep(&messageParse, ",");
                mfLookup(atoi(pinum), name);
            }
            /********* STAT **********/
            if (strcmp(opcode, "2") == 0){
                char* inum = strsep(&messageParse, ",");
                mfStat(atoi(inum));
            }
            /********* WRITE **********/
            if (strcmp(opcode, "3") == 0){
                char* inum = strsep(&messageParse, ",");
                char* buffer = strsep(&messageParse, ",");
                char* block = strsep(&messageParse, ",");
                mfWrite(atoi(inum), buffer, atoi(block));
            }
            /********* READ **********/
            if (strcmp(opcode, "4") == 0){
                char* inum = strsep(&messageParse, ",");
                char* buffer = strsep(&messageParse, ",");
                char* block = strsep(&messageParse, ",");
                mfRead(atoi(inum), buffer, atoi(block));
            }
            /********* CREAT **********/
            if (strcmp(opcode, "5") == 0){
                char* pinum = strsep(&messageParse, ",");
                char* type = strsep(&messageParse, ",");
                char* name = strsep(&messageParse, ",");
                mfCreat(atoi(pinum), atoi(type), name);
            }
            /********* UNLINK **********/
            if (strcmp(opcode, "6") == 0){
                char* pinum = strsep(&messageParse, ",");
                char* name = strsep(&messageParse, ",");
                mfUnlink(atoi(pinum), name);
            }
            /********* SHUTDOWN **********/
            if (strcmp(opcode, "7") == 0){
                shutdown = 1;
            }
        }
            

            /********** SHUTDOWN SEQUENCE ********/
            if (shutdown > 0) {
                sprintf(reply, "0");
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
                    exit(0); // success
                }
            }
            /***** WRITE REPLY BACK TO CLIENT *******/
            sprintf(reply, "goodbye world");
            sprint(replyGlobal, "Good morning, good afternoon, and good night");
            rc = UDP_Write(sd, &addr, replyGlobal, BUFFER_SIZE);
	    printf("server:: reply\n");
	} 
    }
    return 0; 
}
