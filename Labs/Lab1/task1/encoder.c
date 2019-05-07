#include <stdio.h>
#include <string.h>

int encrypt(int c, int index, char *code, int len, int mode){
    if(mode==2){
        c=(c+(int)code[(index%len)]);
        c%=128;
    }
    else{
        c=(c-(int)code[(index%len)]);
        c%=128;
        if(c<0)
            c=c*(-1);
    }
     return c;
}


int main(int argc, char**argv){
    FILE* f;
    f=stdin;
    char c=0;
    int mode=0;
    char * code2;
    int flag=0;
    int asize=0;
    int j=0;
    

    for(int i=1;i<argc;i++){
        if(strncmp(argv[i],"-i",2)==0){
            char * filename=argv[i]+2;
            if((f=fopen(filename,"r"))==NULL){
                    printf("file %s cannot be opened\n", filename);
                    return 1;
            }
        }
        else if(strncmp(argv[i],"-D",2)==0){
             flag=1;
             for(int k=1;k<argc;k++){
                fprintf(stderr, "%s ",argv[k]);
                
             }
            fprintf(stderr, "\n");

        }
        else if(strncmp(argv[i],"+e",2)==0){
            mode=2;
            asize=strlen(argv[i])-2;
            code2=argv[i]+2;
        }
        else if(strncmp(argv[i],"-e",2)==0){
            mode=3;
            asize=strlen(argv[i])-2;
            code2=argv[i]+2;
        }
        else{
            printf("illegal arguement : %s \n", argv[i]);
            return 1;
        }
    }
       c=fgetc(f);     
       while(c!=EOF){ 
           int endline=0;
            if(flag==1){
                fprintf(stderr, "%#04x", c);
                fprintf(stderr, "\t");
            }
            if(mode==2){
            if((char)c=='\n'){
                    endline=1;
                    c=encrypt(c,j,code2,asize,mode);
                    fprintf(stdout,"%c", c);
                    c = '\n';
                    j=0;

                }
                else{
                    c=encrypt(c,j,code2,asize,mode);
                    fprintf(stdout,"%c", c);
                    j++;
                    if(j==asize){
                        j=0;
                    }
                }
            }
            else if(mode==3){
                if((char)c=='\n'){
                    endline=1;
                    j=0;
                }
                else{
                    c=encrypt(c,j,code2,asize,mode);
                    fprintf(stdout,"%c", c);
                        j++;
                        if(j==asize){
                            j=0;
                        }
                }
            }
            else if(mode==0){
               if((char)c=='\n'){
                    endline=1;
                }
                else{
                if(c>='A'&&c<='Z'){
                    c=c+'a'-'A';
                }
                fprintf(stdout,"%c", c);
                }
            }
            if(flag==1){
                fprintf(stderr, "%#04x\n",c);
            }
           if(endline){
              fprintf(stdout,"\n");
            }   
            c=fgetc(f);      
        }
        
    if(f!=stdin)
       fclose(f);
}
