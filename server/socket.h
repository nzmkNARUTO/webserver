#define LISTENQ 50 //listening waiting queue

/**
 * @brief create a socket with a given port
 * to let the server work
 * @param port the port to use
 * @return int socket id
 */
int makeServerSocket(int port);