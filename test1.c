#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define BUFFER_SIZE (1000)

// Message struct used for request and replies from the server
struct message {
    char opcode[BUFFER_SIZE];
    char msg[BUFFER_SIZE];
    //todo, add more fields for relevant args   
};

struct message *msg = NULL;

int main(int argc, char* argv[]) {
    //int rc = MFS_Init("royal-29.cs.wisc.edu", 1000);
    //printf("hello world\n");
    struct message *msg = (struct message*) malloc(sizeof(struct message)); 
    strcpy(msg->opcode, "5");
    strcpy(msg->msg, "5. Test on Opcode 5");

    printf("Here is msg->opcode %s\n", msg->opcode );
    printf("Here is msg->msg %s\n", msg->msg );

    // turn struct into string and print!
    char bitStr[BUFFER_SIZE];
    strcpy(bitStr, msg->opcode);
    strcat(bitStr,  msg->msg);
    char structToString[sizeof(struct message)];
    memcpy(structToString, bitStr, sizeof(struct message));
    printf("Here is the final result!: %s\n", structToString);

    return 0;
}
