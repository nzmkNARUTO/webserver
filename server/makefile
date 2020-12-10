all:server socket server.o socket.o
	@gcc server.o socket.o -o server
server:server.c server.h socket
	@gcc -c server.c -o server.o
socket:socket.c socket.h
	@gcc -c socket.c -o socket.o
clean:
	@-rm server *.o