#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "Dis.h"

int  main(int argc, char** argv)
{
    char buffer[8], c;
    FILE *fp;
    fp = fopen(argv[1],"rb");
    if(fp == NULL)
    {
        printf("Error in file\n");
        exit(1);
    }
    fread(buffer,8,1,fp);
    c = getc(fp);
    if(c != 0x63) {
        printf("Not a code object !\n");
        exit(1);
    }
    code_object(fp);
    fclose(fp);
    return 0;
}
void code_object(FILE *fp)
{
    int c;
    int argcount;
    int nlocals;
    int stacksize;
    int flags;
    long size ;
    unsigned char buffer[10000];
    int constants[100];
    int n_const = 0;
    char *names[100];
    int n_name = 0;
    char *file_name;
    char *name;
    int first_lno;

    argcount = r_long(fp);
    nlocals = r_long(fp);
    stacksize  = r_long(fp);
    flags = r_long(fp);
    //printf("argcount %d\nnlocals %d\nstacksize %d\nflags %x\n", argcount, nlocals, stacksize, flags);
    getc(fp);
    size = r_long(fp);
    fread(buffer,size,1,fp);
    get_tuple(fp, constants, &n_const, names, &n_name);
    get_tuple(fp, constants, &n_const, names, &n_name);
    get_tuple(fp, constants, &n_const, names, &n_name);
    get_tuple(fp, constants, &n_const, names, &n_name);
    get_tuple(fp, constants, &n_const, names, &n_name);

    dis(buffer, size, constants, n_const, names, n_name);

    c = getc(fp);
    size = r_long(fp);
    file_name = get_strings(fp, size);

    c = getc(fp);
    size = r_long(fp);
    name = get_strings(fp, size);

    first_lno = (int)r_long(fp);

    c = getc(fp);
    size = r_long(fp);
    get_strings(fp, size);
    printf("\n\n");
}

void get_tuple(FILE *fp, int constants[], int *n_const, char *names[], int *n_name)
{
    int c, i,n,size;
    c = getc(fp);
    if(c != 0x28)
        return;
    n = (int)r_long(fp);
    for(i = 0; i < n; i++) {
        c = getc(fp);
        switch(c) {
        case 0x69:
            constants[*n_const] = (int)r_long(fp);
            *n_const += 1;
            break;
        case 0x74:
            size = r_long(fp);
            names[*n_name] = get_strings(fp,size);
            *n_name += 1;
            break;
        case 0x63:
            code_object(fp);
            break;
        case 0x52:
            size = r_long(fp);
            break;
        default :
            break;
        }
    }


}

void dis(unsigned char buffer[], int size, int constants[], int n_const, char *names[], int n_name)
{
    int i = 0, operand,c;
    while(i < size) {
        printf("    %d  ",i);
        c = buffer[i];
        if(c < 90) {
            printf("%s\n",opcodes[c]);
            i++;
        }
        else {
            operand = get_operand(buffer,i+1);
            if(strstr(opcodes[c],"NAME") != NULL)
                printf("%s\t%d (%s)\n",opcodes[c],operand,names[operand]);
            else if(strstr(opcodes[c],"CONST") != NULL)
                printf("%s\t%d (%d)\n",opcodes[c],operand,constants[operand]);
            else
                printf("%s\t%d\n",opcodes[c],operand);
            i += 3;
        }
    }
}
long r_long(FILE *fp)
{
    long x;
    x = getc(fp);
    x |= (long)getc(fp) << 8;
    x |= (long)getc(fp) << 16;
    x |= (long)getc(fp) << 24;
    return x;
}
int get_operand(unsigned char buffer[], int start)
{
    int x;
    x = buffer[start++];
    x |= buffer[start] << 8;
    return x;
}
char *get_strings(FILE *fp, int size)
{
    char temp[100], *c;
    fread(temp,size,1,fp);
    int i =0;
    temp[size] = '\0';
    c = (char *)malloc(size+1);
    strcpy(c,temp);
    return c;
}
