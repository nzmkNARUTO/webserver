all:server socket
	@echo "LINK:server.o socket.o => server"
	@gcc server.o socket.o -o server
server:server.c server.h socket
	@echo "compile:server.c server.h => server.o"
	@gcc -c server.c -o server.o
socket:socket.c socket.h
	@echo "compile:socket.c socket.h => socket.o"
	@gcc -c socket.c -o socket.o
clean:
	@-rm server *.o