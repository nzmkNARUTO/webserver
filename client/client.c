#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
    int i = execl("/home/bill/webserver/server","/test.sh",NULL);
    printf("%d\n",i);
    return 0;
}