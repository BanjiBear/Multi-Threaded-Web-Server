# [2020] Multi-Threaded Web Server
## Introduction
The Multi-Threaded Web Server project is an implementation of socket programming. The goals are to be familiar with the Computer Networking, HTTP Protocol, and the OSI Layers. The requirements of this project includes:
- [x] Develop a multi-threaded web server
- [x] display the contents of the HTTP request messages that the web server receives
- [x] generate an appropriate response to the request according to the HTTP protocol
- [x] log file to record statistics of the client requests
- [x] two request methods, GET, HEAD
- [x] three respond types, 200 OK, 400 Bad Request, 404 File Not Found
- [x] Be able to define your own port number

<img width="1440" alt="Screenshot 2024-05-20 at 9 42 15 PM" src="https://github.com/BanjiBear/Multi-Threaded-Web-Server/assets/70761188/6dc9a095-1b70-41a7-8071-40d856ee7d32">
<img width="1440" alt="image" src="https://github.com/BanjiBear/Multi-Threaded-Web-Server/assets/70761188/37317a9f-fd22-4551-9fb1-55f678cf65a2">
<img width="1440" alt="Screenshot 2024-05-20 at 9 43 56 PM" src="https://github.com/BanjiBear/Multi-Threaded-Web-Server/assets/70761188/60b6fd3e-effa-4a8e-a77c-30b5a87fdcaf">

## Installation and Execution
The web server program can be run on any platform that supports compiling and executing C programs. Please make sure to have proper C compiler before starting the program.

To compile the program, please navigate to the project folder and enter the following command in terminal
```bash
make server
```
or
```bash
cc server.c -o server
```
The program takes in one parameter, that is, user-define port number. This number is not limited and can be any four-digit number, e.g. 2545. Enter the following command in the terminal to start a local server.
```bash
./server <server port>
```
For example,
```bash
./server 2545
```
After the execution of the program, open any web browser and type in:
```
localhost:2545/
```
After ```/```, type in any file name that exist in the same folder with the server code. The content of the file can then be seen on the browser. Meanwhile, the terminal constantly outpus the client request and the sever response. An additional log file named ```Client request.txt``` will also be generated automatically logging all the access information as requested. Please refer to the file ```Client request sample.txt``` in this repository if needed.

<img width="1440" alt="Screenshot 2024-05-20 at 10 09 29 PM" src="https://github.com/BanjiBear/Multi-Threaded-Web-Server/assets/70761188/824f307f-a6b0-4a09-b528-1b0c14d017ee">

## Implementation
In order to form or create a web server that initiates web sockets, there are 6 steps, 
1. Create a socket
2. Bind to a access
3. Listen to a port
4. Accept the connection from a client
5. Send and receive data
6. and finally Close the created socket(s).

The program leverages C libraries to built and binds local sockets and allows to communicate and fetch local files over the internet. The socket is created by:
```C
newSocket = socket(AF_INET, SOCK_STREAM, 0);
```
The function takes in three parameters the domain, type, and protocol respectively. The domain here is ```AF_INET``` refers to the address family that is used to designate the type of addresses the socket can communicate with (in this case, Internet Protocol v4 addresses). The communication type ```SOCK_STREAM``` refers to TCP communication, which is more reliable and connection-oriented. The last parameter 0 refers to the Internet Protocol(IP).

Second step binding is done by:
```C
server.sin_family = AF_INET;           // Set up address for binding
server.sin_addr.s_addr = INADDR_ANY;   // Automatically be filled with current host's IP address
server.sin_port = htons(portNum);      // The network byte order of the port number
int Bind = bind(newSocket, (struct sockaddr *) &server, sizeof(server));
if(Bind < 0){
	printf("Error on binding!\n");
	exit(-1);
}
```
The bind function binds the socket to the address and port number specified in addr. Since the IP address has been set to ```INADDR_ANY```, the socket is not bind to any specific IP hence will only bind with localhost.

For listening, a while loop is created to constantly listen to incoming requests.
```C
int Listen = listen(newSocket, SOMAXCONN);
if(Listen < 0){
	printf("Listen failed!\n");
	exit(-1);
}
```
The ```listen()``` function reads in 2 parameters, a new socket and the maximum number of sockets allowed to be queued. Once a request is incoming, the socket accepts it and finally establish a connection
```C
newSocket2 = accept(newSocket, (struct sockaddr *) &server, (socklen_t*) &addrlen);
int valread = read(newSocket2 , request, 2048);
```

