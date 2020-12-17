#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(){
    int i = execl("test",NULL);
    if(i < 0){
        perror("error\n");
    }
    return 0;
}