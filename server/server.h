#define PORT 8888

void processRequest(char* request, int fd);
void doWrong(int fd);
void do404(char* argument, int fd);
void doLs(char* argument, int fd);
void doExe(char* argument, int fd);
void doCat(char* argument, int fd);
int isExist(char* argument);
int isDir(char* argument);
int isSh(char* argument);
void header(FILE *fp,char *content_type);
char * fileType(char* argument);
void read_from(FILE *fp);
void welcome(int fd);