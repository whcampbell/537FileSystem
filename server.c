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
char* thisDir = ".";
char* parDir = "..";

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
    int dpoint = traverse(pinum);
    lseek(image, dpoint, SEEK_SET);
    struct inode* node = 0;
    read(image, node, sizeof(struct inode));
    int found = -1;
    int i;
    // iterate through for matching name
    // send back corresponding inode
    for (i = 0; i < 14; i++) {
        lseek(image, node->pointers[i], SEEK_SET);
        int j;
        for (j = 0; j < 120; j++){ // 4096 (total bytes in a data block) / 32 (size of dir entry) = 128 poss dir entries
            struct __MFS_DirEnt_t* dirEntry = 0;
            read(image, dirEntry, sizeof(struct __MFS_DirEnt_t));
            if (strcmp(dirEntry->name, name) == 0 && dirEntry->inum > -1 ){
                // FOUND IT!!!, set the message to success make the client jump for joy!
                found = 1;
            }
        }
    }
        ///didnt find it, set appropriate message to be send back to client
    if (found == -1) {
        strcpy(replyGlobal, "-1");   
    }

    strcpy(replyGlobal, "0");
    return -1;
}


int mfLookup(int pinum, char* name) {      
    int dpoint = traverse(pinum);
    // iterate through for matching name
    // send back corresponding inode
}

int mfRead(int inum, char* buffer, int block) {
    strcpy(replyGlobal, "-1");
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
    strcpy(replyGlobal, "-1");  
    return -1;
}


int mfCreat(int pinum, int type, char* name) {
    //check if name exists in parent
    int exists = mfLookup(pinum, name);
    if (exists) {
	strcpy(replyGlobal, "0");
	return 0;
    }

    // pick new inum
    int i;
    int j;
    int prevPiece = pieces[0];
    int mapPiece[16];
    int inum = -1;
    for (i = 1; i < 256; ++i) { 
	if (pieces[i] == 0) { // we're entering the land of empty pieces
	    lseek(image, prevPiece, SEEK_SET); // check if the last piece is entirely full
	    read(image, mapPiece, sizeof(int) * 16);
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

    struct inode* newNode = 0;
    // if directory, add directory data block and set inode accordingly
    if (type == MFS_DIRECTORY) {

	struct __MFS_DirEnt_t entries[128];
	strcpy(entries[0].name, thisDir);
	entries[0].inum = inum;
	strcpy(entries[1].name, parDir);
	entries[1].inum = pinum;

	int z;
	for (z = 2; z < 128; ++z) {
	    entries[z].inum = -1;
	}

	write(image, entries, 128 * sizeof(struct __MFS_DirEnt_t));

	newNode->type = type;
	newNode->size = 4096;
	newNode->pointers[0] = end;
	end += 4096;
	lseek(image, end, SEEK_SET);
    } else {
	newNode->size = 0;
	newNode->type = type;
    }

    // add inode to end of list with map piece
    write(image, newNode, sizeof(struct inode));
    mapPiece[j] = end;
    end += 64;
    write(image, mapPiece, 64);
    pieces[i] = end;
    end += 64;

    // add to checkpoint region - update in mem and on disk
    lseek(image, 0, SEEK_SET);
    write(image, &end, sizeof(int));
    write(image, pieces, 256 * sizeof(int));

    // add to parent directory
    int pinodePoint = traverse(pinum);
    int k;
    int l;
    lseek(image, pinodePoint, SEEK_SET);
    struct inode* pinode = 0;
    read(image, pinode, sizeof(struct inode));
    for(k = 0; k < 14; ++k) {
	lseek(image, pinode->pointers[k], SEEK_SET);
	for(l = 0; l < 128; ++l) {
	    struct __MFS_DirEnt_t* dirBucket = 0;
	    read(image, dirBucket, sizeof(struct __MFS_DirEnt_t));
	    if (dirBucket->inum == -1) {
		dirBucket->inum = inum; // the "inum" variable from all the way back there ^
		strcpy(dirBucket->name, name);
	    } else {
		continue;
	    }
	}
    }

    // send back confirm code
    strcpy(replyGlobal, "0");
    return 0;
}

int mfStat(int inum) {
    int ipointer = traverse(inum);
    lseek(image, ipointer, SEEK_SET);
    int* size = 0;
    read(image, size, 4);
    int* type = 0;
    read(image, type, 4);

    //prep the buffer for stat data to be sent back from this inode/file
    char bitStr[BUFFER_SIZE];
    // convert ints to char*
    char sizeAsStr[5];
    char typeAsStr[2];
	sprintf(sizeAsStr, "%d", *size);
    sprintf(typeAsStr, "%d", *type);
    strcpy(bitStr, "0");
    strcat(bitStr, ",");
    strcat(bitStr, typeAsStr);
    strcat(bitStr, ",");
    strcat(bitStr, sizeAsStr);
    char sendMsg[sizeof(struct message)];
    memcpy(sendMsg, bitStr, sizeof(struct message)); // might seem redundant right now but useful later for multiple strings (I think)
    strcpy(replyGlobal, sendMsg); // set the global reply
    return 0;
}


int mfUnlink(int pinum, char* name) {
 //   int dpoint = traverse(pinum);
//iterate through directory to find name
//swap its inode to -1
//send back reply
    strcpy(replyGlobal, "-1");
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

    // open new file
    image = open(path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (image < 0) {
	printf("unable to create file\n");
	exit(1);
    }

    // create root directory
    int* root = malloc(128 * sizeof(struct __MFS_DirEnt_t));

    struct __MFS_DirEnt_t self;
    struct __MFS_DirEnt_t parent;
    char* selfName = ".";
    char* parentName = "..";
    strcpy(self.name, selfName);
    strcpy(parent.name, parentName);
    //TODO finish initializing root with own inodes, inode block, map piece

    // write all to sys image file
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
                sprintf(replyGlobal, "0");
                rc = UDP_Write(sd, &addr, replyGlobal, BUFFER_SIZE);
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
            /***** WRITE REPLY BACK TO CLIENT (REPLY SET IN HELPER FUNCTIONS) *******/
            rc = UDP_Write(sd, &addr, replyGlobal, BUFFER_SIZE);
	    printf("server:: reply\n");
	} 
    }
    return 0; 
}
