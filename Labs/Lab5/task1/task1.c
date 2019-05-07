#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "LineParser.h"
#include <linux/limits.h>
#include <unistd.h>

int debugflag=0;

void execute(cmdLine *pCmdLine){
    int status;
    int retInfo;
    int cdInfo;
    char *command;
    command=pCmdLine->arguments[0];
    if(strcmp(command, "quit")==0) {
        freeCmdLines(pCmdLine);
        exit(0);
    }
    if(strcmp(command, "cd")==0) {
        cdInfo=chdir(pCmdLine->arguments[1]);
        if(cdInfo==-1) {
            perror("couldn't activate cd");
        }
        return;
    }

    pid_t procId;
    procId=fork();
    if(procId>0) {
        if(pCmdLine->blocking==1){
            retInfo=waitpid(procId, &status , 0);
            if(retInfo<=-1){
                perror("couldn't wait");
                freeCmdLines(pCmdLine);
                exit(1);
            }
        }
        return;
    }
    if (debugflag==1) {
        fprintf(stderr, "%s %d\n", "PID: ", (int) getpid());
        fprintf(stderr, "%s %s\n", "Executing command: ", pCmdLine->arguments[0]);
    }


    if(execvp(command, pCmdLine->arguments)<0){
        perror("couldn't execute the command");
        freeCmdLines(pCmdLine);
        exit(1);
    }
}

int main(int argc, char **argv) {
    cmdLine *CMDLine;
    char buffer[2048];
    char path[PATH_MAX];
    if(argc>1){
        if(strncmp(argv[1],"-d",2)==0)
            debugflag=1;
    }

    while (1) {
        getcwd(path, PATH_MAX);
        printf("%s :\n", path);
        fgets(buffer, 2048, stdin);

        CMDLine = parseCmdLines(buffer);
        execute(CMDLine);
        freeCmdLines(CMDLine);
    }
    return 0;
}
