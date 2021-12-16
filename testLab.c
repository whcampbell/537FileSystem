#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "mfs.h"

int main(int argc, char* argv[]) {
    int rc = MFS_Init("royal-29.cs.wisc.edu", 1000);
    //printf("hello world\n");
    return rc;
}
