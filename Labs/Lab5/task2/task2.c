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
#include <signal.h>


#define TERMINATED  -1
#define RUNNING 1
#define SUSPENDED 0



typedef struct process{
    cmdLine* cmd;                         /* the parsed command line*/
    pid_t pid; 		                  /* the process id that is running the command*/
    int status;                           /* status of the process: RUNNING/SUSPENDED/TERMINATED */
    struct process *next;	                  /* next process in chain */
} process;

int debugflag=0;
process** proc_list=NULL;


void updateProcessList(process **process_list) {   /*go over the process list, and for each process check if it is done*/
    if(process_list==NULL) {
        return;
    }
    process *temp = *process_list;
    int retInfo;
    int status;
    while (temp != NULL) {
        if (temp->status == RUNNING) {
            retInfo = waitpid(temp->pid, &status, WNOHANG);
            if(retInfo != 0 && retInfo != -1)
                temp->status=-1;
        }
        temp = temp->next;
    }
}

/* find the process with the given id in the process_list and change its status to the received status.*/
void updateProcessStatus(process** process_list, int pid, int status) {
    if(process_list==NULL) {
        return;
    }
    process *temp=*process_list;
    while(temp!=NULL){
        if(temp->pid==pid){
            temp->status=status;
        }
        temp=temp->next;
    }
}

void addProcess(process** process_list, cmdLine* cmd, pid_t pid){
    process *processIt= malloc(sizeof(process));
    processIt->cmd=cmd;
    processIt->pid=pid;
    processIt->status=RUNNING;
    processIt->next=NULL;
    if(process_list==NULL){
        process_list=malloc(sizeof(process*));
        *process_list=processIt;
    }
    else{
        process *temp=*process_list;
        while(temp->next!=NULL){
            temp=temp->next;
        }
        temp->next=processIt;
    }
    proc_list=process_list;
}

char* statusToString(int status){
    char * print;
    if(status==-1)
        print="Terminated";
    if(status==1)
        print="Running";
    if(status==0)
        print="Suspended";
    return print;
}

void printProcessList(process** process_list){

    if(process_list==NULL){
        printf("there are no processes in the list\n");
        return;
    }
    else{
        updateProcessList(process_list);
        process *prev=NULL;
        process *curr=*process_list;
        process *next=curr->next;
        fprintf(stderr, "%s         %s         %s\n","PID","Command","STATUS");
        while (curr!=NULL){
            fprintf(stderr, "%d         %s         %s\n", curr->pid, curr->cmd->arguments[0], statusToString(curr->status));
                if (curr->status == TERMINATED) {
                    if (prev == NULL) {
                        *process_list=curr->next;
                        freeCmdLines(curr->cmd);
                        free(curr);

                    } else {
                        prev->next = next;
                        freeCmdLines(curr->cmd);
                        free(curr);
                    }
                }
            prev=curr;
            curr=next;
            if(next!=NULL)
                next=next->next;
        }
    }
    if (*process_list==NULL){
        free(*process_list);
        free(proc_list);
        process_list=NULL;
        proc_list=process_list;
    }
}

void freeProcessList(process* process_list) { /* free all memory allocated for the process list.*/
    process *curr=process_list;
    while(curr!=NULL){
        process *temp=curr;
        curr=curr->next;
        freeCmdLines(temp->cmd);
        free(temp);
    }
}

void execute(cmdLine *pCmdLine) {
    int status;
    int cdInfo;
    char *command;
    pid_t procId = -1;
    command = pCmdLine->arguments[0];


    if (strcmp(command, "quit") == 0) {
        freeCmdLines(pCmdLine);
        freeProcessList(*proc_list);
        exit(0);
    } else if (strcmp(command, "procs") == 0) {
        printProcessList(proc_list);
        freeCmdLines(pCmdLine);

    } else if (strcmp(command, "cd") == 0) {
        cdInfo = chdir(pCmdLine->arguments[1]);
        if (cdInfo == -1) {
            perror("couldn't activate cd");
        }
        freeCmdLines(pCmdLine);
    } else if (strcmp(pCmdLine->arguments[0], "suspend") == 0) {
        if(kill(atoi(pCmdLine->arguments[1]), SIGTSTP)==-1) /*kill sends signal to a process */
            perror("error in kill()");
        updateProcessStatus(proc_list, atoi(pCmdLine->arguments[1]), SUSPENDED);
        freeCmdLines(pCmdLine);
    }
    else if (strcmp(pCmdLine->arguments[0], "kill") == 0) {
        if(kill(atoi(pCmdLine->arguments[1]), SIGINT)==-1) /*kill sends signal to a process */
            perror("error in kill()");
        updateProcessStatus(proc_list, atoi(pCmdLine->arguments[1]), TERMINATED);
        freeCmdLines(pCmdLine);
    }
    else if (strcmp(pCmdLine->arguments[0], "wake") == 0) {
        if(kill(atoi(pCmdLine->arguments[1]), SIGCONT)==-1) /*kill sends signal to a process */
            perror("error in kill()");
        updateProcessStatus(proc_list, atoi(pCmdLine->arguments[1]), RUNNING);
        freeCmdLines(pCmdLine);
    }
    else {
        procId = fork();/*creates a new process by duplicating the calling process. The new process, referred to as the child, is an exact duplicate of the calling process, referred to as the parent,  */
        if (procId == -1) {
            perror("fork"); /*produces a message on standard error describing the last error encountered during a call to a system or library function. */
            exit(1);
        }

        if (debugflag == 1) {
            if (procId != 0)
                fprintf(stderr, "%s %d\n %s %d\n", "current PID: ", (int) getpid(), "parent PID:", (int) getppid());
            else {
                fprintf(stderr, "%s %d\n %s %d\n", "child PID: ", (int) getpid(), "parent PID:", (int) getppid());
            }
            fprintf(stderr, "%s %s\n", "Executing command: ", pCmdLine->arguments[0]);
        }

        if (procId!= 0) {
            addProcess(proc_list, pCmdLine, procId);
            if (pCmdLine->blocking == 1) {
                waitpid(procId, &status, 0);
                if(status==0)
                    updateProcessStatus(proc_list, procId, TERMINATED);
            }
            return;
        }
        if (execvp(command, pCmdLine->arguments) < 0) {
            perror("couldn't execute the command");
            freeCmdLines(pCmdLine);
            _exit(1);
        }
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
        getcwd(path, PATH_MAX); /*pathname that is the current working directory of the calling process*/
        printf("%s :\n", path);
        fgets(buffer, 2048, stdin); /*get the request from the user */

        CMDLine = parseCmdLines(buffer); /*create a new cmd line*/
        execute(CMDLine);

    }
    return 0;
}
