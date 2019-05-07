#include "util.h"

#define SYS_EXIT 1
#define STDIN 0
#define STDOUT 1
#define STDERR 2
#define SYS_READ 3
#define SYS_WRITE 4
#define SYS_OPEN 5
#define SYS_CLOSE 6
#define EXIT_FAILURE 0x55

extern int system_call(int,...);

void handle_error(char *msg){
    system_call(SYS_WRITE,STDERR,msg, strlen(msg));
    system_call(SYS_EXIT,EXIT_FAILURE ,msg, strlen(msg));
}

void handleDebugMode(int debug, int systemCallId, int systemCallRetCode, int toRead, int toWrite){
    if(debug){
        if (toRead==0)
            system_call(SYS_WRITE,STDERR,"you are reading from STDIN\n", 27);
        if (toWrite==1)
            system_call(SYS_WRITE,STDERR,"you are writing to STDOUT\n", 26);
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
        system_call(SYS_WRITE,STDERR,"\n",1);
    }
    
}


int main (int argc , char* argv[]) {
    char input[10000];
    int location=0;
    int debug=0;
    int returnValue=0;
    int i=0;
    int toRead=STDIN;
    int toWrite=1;
    char *inputFileName;
    char *outputFileName;
    while(i<argc) {
        if (strncmp(argv[i], "-i", 2) == 0) {
            inputFileName = argv[i] + 2;
            toRead=system_call(SYS_OPEN, inputFileName, 2, 0777);
            if(toRead<0)
                handle_error("can't open the file");
        }
        else if (strncmp(argv[i], "-o", 2) == 0) {
            outputFileName = argv[i] + 2;
            toWrite= system_call(SYS_OPEN, outputFileName, 64|2, 0777);
            if(toWrite<0)
                handle_error("can't open the file");
        }
        else{
            if(strncmp(argv[i], "-D", 2) == 0){
                debug=1;
            }
        }
        i++;
    }

    returnValue=system_call(SYS_READ, toRead, &input[location], 1);
    handleDebugMode(debug,SYS_READ ,returnValue, toRead, toWrite);
    if (returnValue<0)
        handle_error("open");
    while(input[location]!='\n'){
        if(input[location]>='A'&&input[location]<='Z'){
            input[location]=input[location]+'a'-'A';
        }
        location++;
        returnValue=system_call(SYS_READ, toRead, &input[location], 1);
        if (returnValue<0)
            handle_error("read");
        handleDebugMode(debug,SYS_READ ,returnValue, toRead, toWrite);
    }

    returnValue=system_call(SYS_WRITE,toWrite,&input, location+1);
    if (returnValue<0)
        handle_error("read");
    handleDebugMode(debug,SYS_WRITE ,returnValue, toRead, toWrite);


    system_call(SYS_CLOSE,toRead);
    system_call(SYS_CLOSE,toWrite);

return 0;
}
