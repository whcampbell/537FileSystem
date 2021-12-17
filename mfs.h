#ifndef __MFS_h__
#define __MFS_h__

#define MFS_DIRECTORY    (0)
#define MFS_REGULAR_FILE (1)

#define MFS_BLOCK_SIZE   (4096)

typedef struct __MFS_Stat_t {
    int type;   // MFS_DIRECTORY or MFS_REGULAR
    int size;   // bytes
    // note: no permissions, access times, etc.
} MFS_Stat_t;

typedef struct __MFS_DirEnt_t {
    char name[28];  // up to 28 bytes of name in directory (including \0)
    int  inum;      // inode number of entry (-1 means entry not used)
} MFS_DirEnt_t;

// Communication/argument passing protocol: 
//   The first character in the "buffer" string (one of the UDP_Write args) will
//   encode the operation being called for. The encoding is thus:
//   	1: lookup
//   	2: stat
//   	3: write
//   	4: read
//   	5: creat
//   	6: unlink
//   	7: shutdown
//   from then on, the data in the string will contain the arguments which
//   can be parsed in the specific way that call desires
//   (my plan is to have a big switch statement referring to a bunch of helper
//   methods)


int MFS_Init(char *hostname, int port);
int MFS_Lookup(int pinum, char *name);
int MFS_Stat(int inum, MFS_Stat_t *m);
int MFS_Write(int inum, char *buffer, int block);
int MFS_Read(int inum, char *buffer, int block);
int MFS_Creat(int pinum, int type, char *name);
int MFS_Unlink(int pinum, char *name);
int MFS_Shutdown();

#endif // __MFS_h__
