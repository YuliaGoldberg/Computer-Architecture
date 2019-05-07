#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>



typedef struct virus {
    unsigned short SigSize;
    char virusName[16];
    char sig[];
} virus;

typedef struct link link;

struct link {
    link *nextVirus;
    virus *vir;
};

link* jinki=NULL;

void PrintHex(char *buffer, int length){
    int counter=1;
    for(int i=0; i<length;i++){
        printf( "%02X ", buffer[i]& 0xff);
        if(counter%20==0)
            printf("\n");
        counter++;
    }
    printf("\n");
}


/* Print the data of every link in list. Each item followed by a newline character. */
void list_print(link *virus_list){
    link * linki=virus_list;
    while(linki!= NULL) {
        printf("Virus size: %s\n", linki->vir->virusName);
        printf("Virus name: %d\n", linki->vir->SigSize);
        printf("%s\n", "signature:");
        PrintHex(linki->vir->sig, linki->vir->SigSize);

        linki = linki->nextVirus;
    }
}

/* Add a new link with the given data to the list
(either at the end or the beginning, depending on what your TA tells you),
and return a pointer to the list (i.e., the first link in the list).
If the list is null - create a new entry and return a pointer to the entry. */
link* list_append(link* virus_list, virus* data) {
    link * linki=virus_list;
    if (linki == NULL) {
        linki = calloc(1, sizeof(link));
        linki->vir = data;
        linki->nextVirus = NULL;
        return linki;
    } else {
        while (linki->nextVirus != NULL) {
            linki = linki->nextVirus;
        }
        link *minki= calloc(1, sizeof(link));
        linki->nextVirus = minki;
        minki->vir = data;
        minki->nextVirus=NULL;
    }
    return virus_list;
}

/* Free the memory allocated by the list. */
void list_free(link *virus_list){
    link* linki= virus_list;
    link *minki=virus_list;
    while(minki) {
        linki = minki;
        minki = linki->nextVirus;
        free(linki->vir);
        free(linki);
    }
}
void detect_virus(char *buffer, unsigned int size) {
    while (jinki != NULL) {
        for (int i = 0; i < size; ++i) {
            size_t t=jinki->vir->SigSize;
            if (memcmp(&buffer[i], jinki->vir->sig, t) == 0) {
                printf("%s%d\n", "Byte location: ", i);
                printf("%s%s\n", "Virus name:", jinki->vir->virusName);
                printf("%s%d\n\n", "Virus signature size: ", jinki->vir->SigSize);
            }
        }
        jinki = jinki->nextVirus;
    }
}
void kill_virus(char *fileName, int signitureOffset, int signitureSize){
    FILE *virusInspector=NULL;
    virusInspector=fopen(fileName,"r+");
    fseek(virusInspector, signitureOffset, SEEK_SET);

    char *c=calloc((size_t )signitureSize, sizeof(char));
    for (int i = 0; i < signitureSize; ++i) {
        c[i]=(char)0x90;
    }
    fwrite(c, sizeof(char), (size_t )signitureSize, virusInspector);
    free(c);
    fclose(virusInspector);
}


link* readFromFile(char * filename, link * linki){
    FILE *f;
    if((f=fopen(filename,"r"))==NULL){
        printf("file named ' %s ' does not exist!\nplease try again\n\n", filename);
        return NULL;
    }
    char buffer[2];
    while(fread(buffer, 1,2 ,f)) {
        unsigned short p=0;
        p = (((unsigned short) buffer[1]) << 8) | buffer[0];
        virus *myVirus= calloc(1, sizeof(virus) + (p - 18) * sizeof(char));
        myVirus->SigSize = p-(unsigned short)18;
        fread(myVirus->virusName, sizeof(char), 16, f);
        fread(myVirus->sig, sizeof(char),  myVirus->SigSize, f);

        linki=list_append(linki,myVirus);

    }
    fclose(f);
    return linki;
    }


int main(int argc, char **argv) {
    link *linki=NULL;
    FILE *virusDetector=NULL;

    bool islistReady = false;
    char filename[100]={0};
    char buffer[10000]={0};
    long getFileSize=0;
    while (1) {
        printf("%s\n", "Please choose a function:");
        printf("%s\n", "1) Load signatures");
        printf("%s\n", "2) Print signatures");
        printf("%s\n", "3) Detect viruses");
        printf("%s\n", "4) Fix file");
        printf("%s\n", "5) Quit");
        printf("%s\n", "Option: ");
        char a[3];
        int i = atoi(fgets(a, 3, stdin));
        if (i==1){
            printf("%s\n", "please insert file name: ");
            fgets(filename, 100, stdin);
            sscanf(filename, "%s", filename);
            linki=readFromFile( filename, linki);
            islistReady=true;
        }
        else if(i==2){
            if(islistReady) {
                list_print(linki);
            }
        }
        else if(i==3) {
            jinki = linki;
            virusDetector=fopen(argv[1],"r");
            fseek(virusDetector, 0, SEEK_END);
            getFileSize = ftell(virusDetector);
            fseek(virusDetector, 0, SEEK_SET);
            fread(buffer, sizeof(char), getFileSize, virusDetector);
            if(getFileSize>10000) {
                getFileSize = 10000;
            }
            detect_virus(buffer,(unsigned int) getFileSize);
            fclose(virusDetector);
        }
        else if(i==4){

            printf("%s\n", "please insert the starting byte location: ");
            char a[10];
            int signitureOffset= atoi(fgets(a, 10, stdin));
            printf("%s\n", "please insert the signature size: ");
            char b[10];
            int signitureSize= atoi(fgets(b, 10, stdin));

            kill_virus(argv[1], signitureOffset, signitureSize);

        }
        else if(i==5){
            list_free(linki);
            //if(virusDetector!=NULL)
             //   fclose(virusDetector);
            exit(0);
        }
        else{
            printf("%s", "no such option");
        }
    }

}
