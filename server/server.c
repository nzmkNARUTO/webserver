#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include "server.h"
#include "socket.h"

int main(int argc, char **argv){
    int port = PORT;//8888 by default
    if(argc == 1){
        printf("Server will run in port 8888 by default\n");
    }else{
        int c;
        int index;
        while(1){
            c = getopt_long(argc, argv, "hp:", opts, &index);//get options
            if(c < 0)
                break;
            switch (c)
            {
            case 0:
                switch(index){
                    case 1:
                        port = atoi(optarg);
                        break;
                    default:
                        break;
                }
                break;
            case 'p':
                port = atoi(optarg);
                break;
            case 'h':
            default:
                printf("DISCRIPTION:\n");
                printf("\ta tiny web server only provide limit GET service\n");
                printf("\tthe server will run in port 8888 by default\n");
                printf("USAGE:\n");
                printf("\tserver [ -p | --port]( port )\n");
                printf("\t-p , --port (port)\n");
                printf("\t\tYou can chose a available port\n");
                printf("\t\tNotice that if you want to use a port below 2000\n");
                printf("\t\tyou may need a sudo privilege\n");
                exit(0);
                break;
            }
        }
    }

    int sock, fd;
    FILE *fp_in;
    sock = makeServerSocket(port);//make a socket
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

        fp_in=fdopen(fd,"r");
        fgets(request,BUFSIZ,fp_in);
        printf("Reviced a request: %s",request);
        read_from(fp_in);
        processRequest(request, fd);//process request
        close(fd);
    }
    fclose(fp_in);
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

    if(fork() != 0)//create a subprocess
        return;

    strcpy(argument, "./");//add ./ before argument to make it work in current dir
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
    }else if(isOut(argument)){
        printf("Run program\n");
        doExe(argument, fd);
    }else{
        printf("cat\n");
        doCat(argument, fd);
    }
}

void header(FILE *fp,char *content_type){
    fprintf(fp,"HTTP/1.0 200 OK\r\n");
    if(content_type)
        fprintf(fp,"Content-type:%s;charset=UTF-8\r\n",content_type);
    fprintf(fp, "\r\n");
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
    FILE * fp = fdopen(fd,"w");
    header(fp, "text/plain");
    fflush(fp);
    dup2(fd,1);
    dup2(fd,2);//redirect stdout and stderr to fd
    close(fd);
    execl("/bin/ls","ls","-l",argument,NULL);
    perror(argument);
}

char *fileType(char* argument){
    char * cp;
    if((cp = strrchr(argument,'.')) != NULL)
        return cp+1;
    return "";
}

int isOut(char* argument){
    return (strcmp(fileType(argument),"out") == 0);
}

void doExe(char* argument,int fd){
    FILE * fp;
    fp = fdopen(fd,"w");
    header(fp,"text/plain");
    fflush(fp);
    dup2(fd,1);
    dup2(fd,2);
    close(fd);
    execl(argument,NULL);
    perror(argument);
}

void doCat(char* argument,int fd){
    char * extension = fileType(argument);
    char * content = "text/plain";
    FILE * fp, * fpfile;
    int c;

    if(strcmp(extension,"html") == 0 )
        content = "text/html";
    else if(strcmp(extension,"gif") == 0)
        content = "text/gif";
    else if(strcmp(extension,"jpg") == 0)
        content = "text/jpeg";
    else if(strcmp(extension,"jpeg") == 0)
        content = "text/jpeg";
    else if(strcmp(extension,"ico") == 0)
        content = "image/x-icon";

    fp = fdopen(fd,"w");
    fpfile = fopen(argument,"r");
    if(fp != NULL && fpfile != NULL)
    {
        header(fp,content);
        while((c = getc(fpfile)) != EOF)
            putc(c,fp);
        fclose(fpfile);
        fclose(fp);
    }
}