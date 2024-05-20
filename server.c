// CHEN Yi pu
// 19084858d
// COMP2322 Project

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
// Not sure whether the following libraries are needed
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/uio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <fcntl.h>

/* For server socket:
	1. Create a socket
	2. Bind to a access
	3. Listen to a port
	4. Accept the connection from a client
	5. Send and receive data
	6. Close
*/


void responseMessage(int newSocket2, char request[2048], char Header[5], char fileName[1024], char
	version[1024], char display[1024], char hostName[1024]){

	char data_to_send[1024], path[1024];
	char *directory;
	int fd, bytes_read;

	FILE *eventLog;
	eventLog = fopen("Client request.txt", "a");

	directory = getenv("PWD");
	//printf("%s\n", directory);

	strcpy(path, directory);
	strcpy(&path[strlen(directory)], fileName);
	printf("file: %s\n", path);
	printf("\n");

	//printf("%s\n", request);
	printf("%s\n", request);
	if ( strncmp(Header, "GET", 3) == 0 ){
		if ( strncmp( version, "HTTP/1.0", 8) != 0 && strncmp( version, "HTTP/1.1", 8) != 0 ){
			write(newSocket2, "HTTP/1.0 400 Bad Request\n", 25);
			//eventLog = fopen("Client request.txt", "w");
			fprintf(eventLog, "%s/127.0.0.1 %s 400 Bad Request\n", hostName, version);
			//fclose (eventLog);
		}
		else{
			if ( (fd = open(path, O_RDONLY)) != -1 ){
				send(newSocket2, "HTTP/1.0 200 OK\n\n", 17, 0);
				//eventLog = fopen("Client request.txt", "w");
				time_t currentTime;
				time(&currentTime);
				char time[100] = {0};
				for(int i = 0; i < 24; i++){
					time[i] = ctime(&currentTime)[i];
				}

				struct stat attrib;
				stat(path, &attrib);
				char modify[50];
				strftime(modify, 50, "%Y-%m-%d %H:%M:%S", localtime(&attrib.st_mtime));

				fprintf(eventLog, "%s/127.0.0.1 ", hostName);
				fprintf(eventLog, "%s Last modified at: %s %s 200 OK\n", time, modify, display);
				fclose (eventLog);
				modify[0] = 0;
				while ( (bytes_read=read(fd, data_to_send, 1024))>0 ){
					write (newSocket2, data_to_send, bytes_read);
				}
			}
			else{
				write(newSocket2, "HTTP/1.0 404 Not Found\n", 23); //FILE NOT FOUND
				//eventLog = fopen("Client request.txt", "w");
				fprintf(eventLog, "%s/127.0.0.1 %s 404 Not Found\n", hostName, display);
				//fclose (eventLog);
			}
		}
	}

	//Closing SOCKET
	fclose (eventLog);
	shutdown (newSocket2, SHUT_RDWR);
	close(newSocket2);
}


int main(int argc, char *argv[]){

	int newSocket;
	int newSocket2;
	int portNum;
	struct sockaddr_in server, client;
	int addrlen = sizeof(server);
	// Step 1: Create a socket
	// socket(int domain, int type, int protocol)
	newSocket = socket(AF_INET, SOCK_STREAM, 0);

	if(argc < 2){
		printf("Error! Input format error!\n");
		printf("Input format: ./server 2545\n");
		exit(-1);
	}

	if(argc > 2){
		printf("Error! Input format error!\n");
		printf("Input format: ./server 2545\n");
		exit(-1);
	}
	// Error if socket cannot be created
	if(newSocket < 0){
		printf("Error occurs at creating new socket!\n");
		exit(-1);
	}

	char hostName[1024] = {"tommy@Tommyde-MBP"};
	portNum = atoi(argv[1]);

	// Step 2: Bind to a access
	/* Reference website:
		https://www.geeksforgeeks.org/socket-programming-cc/
		https://www.bogotobogo.com/cplusplus/sockets_server_client.php
	*/
	server.sin_family = AF_INET;			// Set up address for binding
	server.sin_addr.s_addr = INADDR_ANY;	// Automatically be filled with current host's IP address
	server.sin_port = htons(portNum);		// The network byte order of the port number

	// bind() passes three parameters
	// file descriptor, the address, and the length of the address
	int Bind = bind(newSocket, (struct sockaddr *) &server, sizeof(server));
	if(Bind < 0){
		printf("Error on binding!\n");
		exit(-1);
	}

	printf("\n");

	// Step 3: Listen
	while(1){
		int count = 0;
		int second = 0;
		char request[2048] = {0};
		char fileName[1024] = {0}, display[1024] = {0};
		int Listen = listen(newSocket, SOMAXCONN);
		if(Listen < 0){
			printf("Listen failed!\n");
			exit(-1);
		}
		newSocket2 = accept(newSocket, (struct sockaddr *) &server, (socklen_t*) &addrlen);
		int valread = read(newSocket2 , request, 2048);
		//printf("%s\n", request);

		char Header[5] = {0};
		for(int i = 0; i < 3; i++){
			Header[i] = request[i];
		}

		for(int i = 0; i < 2048; i++){
			if(request[i] == '/'){
				count++;
			}
			if(count == 2){
				second = i;
				break;
			}
		}
		//printf("%d\n", second);

		int index = 5;
		for(int i = 0; i < 1024; i++){
			if(index == second - 5){
				break;
			}
			display[i] = request[index];
			index++;
		}

		index = 4;
		for(int i = 0; i < 1024; i++){
			if(index == second - 5){
				break;
			}
			fileName[i] = request[index];
			index++;
		}
		//printf("%s\n", fileName);

		char version[1024] = {0};
		index = second - 4;
		for(int i = 0; i < 1024; i++){
			if(index == second + 4){
				break;
			}
			version[i] = request[index];
			index++;
		}

		//printf("%s\n%s\n%s\n", Header, fileName, version);
		responseMessage(newSocket2, request, Header, fileName, version, display, hostName);
	}
}
