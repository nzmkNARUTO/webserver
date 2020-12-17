#include <unistd.h>
#include <getopt.h>
#define PORT 8888

int opt;
static struct option opts[] = {//getopt_long choices
    {"help", 0, NULL, 'h'},
    {"port", 1, &opt, 1},
    {0,0,0,0}
};

/**
 * @brief process a request using subprocess
 * 
 * @param request the request string
 * @param fd request connection
 */
void processRequest(char* request, int fd);

/**
 * @brief if request is not implement
 * 
 * @param fd request connection
 */
void doWrong(int fd);

/**
 * @brief if the content is not avaliable
 * 
 * @param argument request content
 * @param fd request connection
 */
void do404(char* argument, int fd);

/**
 * @brief list dir using ls -l command
 * 
 * @param argument request path
 * @param fd request connection
 */
void doLs(char* argument, int fd);

/**
 * @brief execute an executable program
 * and return the result to client
 * @param argument request program
 * @param fd request connection
 */
void doExe(char* argument, int fd);

/**
 * @brief mainly used by transmit some stream data
 * like jpg/jpeg etc.
 * @param argument request content
 * @param fd request connection
 */
void doCat(char* argument, int fd);

/**
 * @brief if the content is exist
 * 
 * @param argument content path
 * @return int 1 if exist, 0 if not
 */
int isExist(char* argument);

/**
 * @brief if the request is a dir
 * 
 * @param argument request
 * @return int 1 if exist, 0 if not
 */
int isDir(char* argument);

/**
 * @brief if the request is an executable program
 * 
 * @param argument request
 * @return int 1 if is, 0 if not
 */
int isOut(char* argument);

/**
 * @brief add a html header to normals
 * 
 * @param fp connection file
 * @param content_type content type in response head
 */
void header(FILE *fp,char *content_type);

/**
 * @brief judge the file type
 * 
 * @param argument file name
 * @return char* file type
 */
char * fileType(char* argument);

/**
 * @brief read until /r/n
 * 
 * @param fp file
 */
void read_from(FILE *fp);

