#include "util.h"


#define SYS_EXIT 1
#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_GETDENTS 141
#define EXIT_FAILURE 0x55

extern int system_call(int,...);

enum
{
    DT_UNKNOWN = 0,
# define DT_UNKNOWN	DT_UNKNOWN
    DT_FIFO = 1,
# define DT_FIFO	DT_FIFO
    DT_CHR = 2,
# define DT_CHR		DT_CHR
    DT_DIR = 4,
# define DT_DIR		DT_DIR
    DT_BLK = 6,
# define DT_BLK		DT_BLK
    DT_REG = 8,
# define DT_REG		DT_REG
    DT_LNK = 10,
# define DT_LNK		DT_LNK
    DT_SOCK = 12,
# define DT_SOCK	DT_SOCK
    DT_WHT = 14
# define DT_WHT		DT_WHT
};

struct linux_dirent {
    unsigned long  d_ino;     /* Inode number */
    unsigned long  d_off;     /* Offset to next linux_dirent */
    unsigned short d_reclen;  /* Length of this linux_dirent */
    char           d_name[];  /* Filename (null-terminated) */
    /* length is actually (d_reclen - 2 -
       offsetof(struct linux_dirent, d_name)) */
};


void handle_error(char *msg){
    system_call(SYS_WRITE,STDERR,msg, strlen(msg));
    system_call(SYS_EXIT,EXIT_FAILURE ,msg, strlen(msg));
}

void handleDebugMode(int debug, int systemCallId, int systemCallRetCode){
    if(debug){
        char * sysCallId;
        sysCallId=itoa(systemCallId);
        system_call(SYS_WRITE,STDERR,"There was a system call. here are the details: \n", 48);
        system_call(SYS_WRITE,STDERR,"system call id: ", 16);
        system_call(SYS_WRITE,STDERR,sysCallId, strlen(sysCallId));
        system_call(SYS_WRITE,STDERR,"\n",1);
        char * sysCallRetCode;
        sysCallRetCode=itoa(systemCallRetCode);
        system_call(SYS_WRITE,STDERR,"system call return code: ", 25);
        system_call(SYS_WRITE,STDERR,sysCallRetCode, strlen(sysCallRetCode));
        system_call(SYS_WRITE,STDERR,"\n",1);
    }
    system_call(SYS_WRITE,STDERR,"\n",1);
}

void handleType(char d_type) {
    char *print;
    if(d_type == DT_REG)
        print="regular";
    else if(d_type == DT_DIR)
        print="directory";
    else if(d_type == DT_FIFO)
        print="FIFO";
    else if(d_type == DT_SOCK)
        print="socket";
    else if(d_type == DT_LNK)
        print="symlink";
    else if(d_type == DT_BLK)
        print="block dev";
    else if(d_type == DT_CHR)
        print="char dev";
    system_call(SYS_WRITE,STDOUT,"file type: ", 11);
    system_call(SYS_WRITE,STDOUT,print, strlen(print));
    system_call(SYS_WRITE,STDOUT,"\n",1);
}

int main (int argc , char* argv[]) {
    int debug=0;
    int i=0;
    int p=0;
    char *prefix;
    char buf[8192]={0};
    int nread;
    int fileDescriptor;
    struct linux_dirent *d;
    int bpos=0;
    char d_type;



    while(i<argc) {

        if(strncmp(argv[i], "-p", 2) == 0){
            p=1;
            prefix=argv[i]+2;
        }
        else{
            if(strncmp(argv[i], "-D", 2) == 0){
                debug=1;
            }
        }
        i++;
    }
    int retValue=system_call(SYS_WRITE, STDOUT, "\n", 1);
    if (retValue<0)
        handle_error("write");
    system_call(SYS_WRITE,STDOUT,"Flame 2 strikes! mohahahahahaha \n", 33);
    handleDebugMode(debug, SYS_WRITE, retValue);

    fileDescriptor = system_call(SYS_OPEN, ".", 0, 0777);
    if (fileDescriptor==-1)
        handle_error("open");
    handleDebugMode(debug, SYS_OPEN, fileDescriptor);

    nread = system_call(SYS_GETDENTS, fileDescriptor, buf, 8192);
    if (nread <0)
        handle_error("getDents");
    handleDebugMode(debug, SYS_GETDENTS, nread);

    if(p==1) {
        for (bpos = 32; bpos < nread;) {
            d = (struct linux_dirent *) (buf + bpos);
            if(strncmp(prefix, d->d_name, strlen(prefix))==0){
                d_type = *(buf + bpos + d->d_reclen - 1);
                handleType(d_type);
                retValue=system_call(SYS_WRITE, STDOUT, "file name: ", 11);
                if (retValue <0){
                    handle_error("write");
                }
                system_call(SYS_WRITE, STDOUT, d->d_name, strlen(d->d_name));
                system_call(SYS_WRITE, STDOUT, "\n", 1);
            }
            bpos += d->d_reclen;
        }
    }
    else{
        for (bpos = 32; bpos < nread;) {
            d = (struct linux_dirent *) (buf + bpos);
            system_call(SYS_WRITE, STDOUT, "file name: ", 11);
            if (retValue <0) {
                handle_error("write");
            }
            system_call(SYS_WRITE, STDOUT, d->d_name, strlen(d->d_name));
            system_call(SYS_WRITE, STDOUT, "\n", 1);
            bpos += d->d_reclen;
        }
    }
    system_call(SYS_WRITE, STDOUT, "\n", 1);


return 0;

}
