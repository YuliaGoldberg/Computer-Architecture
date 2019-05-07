#include <stdio.h>
#include <string.h>
#include <stdlib.h>
 
char* map(char *array, int array_length, char (*f) (char)){
  char* mapped_array = (char*)(malloc(array_length*sizeof(char)));
  for(int i=0;i<array_length;i++){
    mapped_array[i]=f(array[i]);
    }
  return mapped_array; 
}

char censor(char c) {
  if(c == '!')
    return '.';
  else
    return c;
}

/* Gets a char c and returns its encrypted form by adding 3 to its value. 
          If c is not between 0x20 and 0x7E it is returned unchanged */
char encrypt(char c){
 if(c<32||c>126)
     return c;
 else{
     return c+3;
 }
}
/* Gets a char c and returns its decrypted form by reducing 3 to its value. 
            If c is not between 0x20 and 0x7E it is returned unchanged */
char decrypt(char c){
 if(c<32||c>126)
     return c;
 else{
     return c-3;
 }
}
/* xprt prints the value of c in a hexadecimal representation followed by a 
           new line, and returns c unchanged. */
char xprt(char c){
    printf("0x%X\n",c);
    return c;
}
/* If c is a number between 0x20 and 0x7E, cprt prints the character of ASCII value c followed 
                    by a new line. Otherwise, cprt prints the dot ('.') character. After printing, cprt returns 
                    the value of c unchanged. */
char cprt(char c){
     if(c>32&&c<126){
         printf("%c\n",c);
     }
     else{
         printf("%c\n",'.');
    }
     return c;
}
/* Ignores c, reads and returns a character from stdin using fgetc. */
char my_get(char c){
    char i=fgetc(stdin);
    return i;
}
char quit(char c){
    exit(0);
}
 
struct fun_desc {
  char *name;
  char (*fun)(char);
};

void printOption(int i, size_t n){
    if(i>=0&&i<n){
        printf("%s\n","Within bounds");
    }
    else{
        printf("%s\n","Not within bounds");
        exit(0);
    }
}
 
int main(int argc, char **argv){
    char array[5];
    array[0]='\0';
    char *carray=array;
    while(1){
        printf("%s\n","Please choose a function:");
        struct fun_desc menu[] = { { "Censor", censor }, { "Encrypt", encrypt },{"Decrypt",decrypt}, {"Print hex", xprt}, {"Print string",cprt}, {"Get string",my_get }, {"Quit", quit}, { NULL, NULL } };
        size_t n = sizeof(menu)/sizeof(menu[0]);
        for(int i=0;i<n-1;i++){
            printf("%d) " "%s\n",i, menu[i].name);
        }
        printf("%s","Option: ");
        char a[n-2];
        int i=atoi(fgets(a, n-2, stdin));
        printOption(i,n);
        if(i==6) {
            free(carray);
            quit('a');
        }
        carray=map(carray, 5, menu[i].fun);
        printf("%s\n", "Done.");
    }
} 

