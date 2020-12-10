#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include "server.h"
#include "socket.h"

int main(){
    int sock, fd;
    FILE *fp_in;
    sock = makeServerSocket(PORT);
    if(sock == -1)
        return -1;
    while (1)
    {
        char request[BUFSIZ];
        memset(request, 0, BUFSIZ);
        fd = accept(sock, NULL, NULL);
        if(fd == -1){
            perror("Connect lost\n");
            break;
        }
        printf("Connect accept!\n");
        //welcome(fd);

        fp_in=fdopen(fd,"r");
        fgets(request,BUFSIZ,fp_in);
        printf("Reviced a request: %s",request);
        //read(fp_in);
        processRequest(request, fd);
        close(fd);
    }
    close(sock);
    return 0;
}

void read_from(FILE *fp)
{
    char buf[BUFSIZ];
    while(fgets(buf,BUFSIZ,fp)!=NULL&&strcmp(buf,"\r\n")!=0);
}

void processRequest(char* request, int fd){
    char command[BUFSIZ], argument[BUFSIZ];
    if(fork() != 0)
        return;
    strcpy(argument, "./");
    if(sscanf(request, "%s%s", command, argument+2) != 2){
        perror("Wrong request");
        return;
    }

    if(strcmp(command, "GET") !=0 ){
        perror("Wrong command, not \"GET\"");
        doWrong(fd);
    }else if(!isExist(argument)){
        perror("Wrong argument");
        do404(argument, fd);
    }else if(isDir(argument)){
        printf("List dir\n");
        doLs(argument,fd);
    }else if(isSh(argument)){
        printf("Run shell script\n");
        doExe(argument, fd);
    }else{
        printf("cat\n");
        doCat(argument, fd);
    }
}

void header(FILE *fp,char *content_type){
    fprintf(fp,"HTTP/1.0 200 OK\r\n");
    if(content_type)
        fprintf(fp,"Content-type:%s\r\n",content_type);
}

void welcome(int fd){
    FILE * fp = fdopen(fd,"w");
    fprintf(fp,"HTTP/1.0 501 Not Implemented\r\n");
    fprintf(fp,"Content_type:text/plain\r\n");
    fprintf(fp,"\r\n");
    fclose(fp);
}
void doWrong(int fd){
    FILE * fp = fdopen(fd,"w");
    fprintf(fp,"HTTP/1.0 501 Not Implemented\r\n");
    fprintf(fp,"Content_type:text/plain\r\n");
    fprintf(fp,"\r\n");

    fprintf(fp,"That command is not yet Implemented");
    fclose(fp);
}

void do404(char* argument,int fd){
    FILE * fp = fdopen(fd,"w");
    fprintf(fp,"HTTP/1.0 404 Not Found\r\n");
    fprintf(fp,"Content_type:text/plain\r\n");
    fprintf(fp,"\r\n");

    fprintf(fp,"The item you requested:%s\r\n is not found \r\n",argument);
    fclose(fp);
}

int isDir(char* argument){
    struct stat info;
    return (stat(argument,&info)!=-1&&S_ISDIR(info.st_mode));
}

int isExist(char* argument){
    struct stat info;
    return (!(stat(argument, &info)==-1));
}

void doLs(char* argument,int fd){
    FILE * fp;
    fp = fdopen(fd,"w");
    fprintf(fp,"text/plain\r\n");
    fflush(fp);

    dup2(fd,1);
    dup2(fd,2);
    close(fd);
    execlp("ls","ls","-l",argument,NULL);
    perror(argument);
}

char * fileType(char* argument){
    char * cp;
    if((cp = strrchr(argument,'.')) != NULL)
        return cp+1;
    return "";
}

int isSh(char* argument){
    return (strcmp(fileType(argument),"sh") == 0);
}

void doExe(char* argument,int fd){
    FILE * fp;
    fp = fdopen(fd,"w");
    header(fp,NULL);
    fflush(fp);
    dup2(fd,1);
    dup2(fd,2);
    close(fd);
    execl("/home/bill/webserver/server",argument,NULL);
    perror(argument);
}

void doCat(char* argument,int fd){
    char * extension = fileType(argument);
    char * content = "text/plain";
    FILE * fpsock, * fpfile;
    int c;

    if(strcmp(extension,"html") == 0 )
        content = "text/html";
    else if(strcmp(extension,"gif") == 0)
        content = "text/gif";
    else if(strcmp(extension,"jpg") == 0)
        content = "text/jpeg";
    else if(strcmp(extension,"jpeg") == 0)
        content = "text/jpeg";

    fpsock = fdopen(fd,"w");
    fpfile = fopen(argument,"r");
    if(fpsock != NULL && fpfile != NULL)
    {
        header(fpsock,content);
        fprintf(fpsock,"\r\n");
        while((c = getc(fpfile)) != EOF)
            putc(c,fpsock);
        fclose(fpfile);
        fclose(fpsock);
    }
}