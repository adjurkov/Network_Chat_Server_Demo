#define WIN32_LEAN_AND_MEAN		

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h> 
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>

#pragma comment (lib, "Ws2_32.lib")

#define DEFAULT_BUFLEN 16
#define DEFAULT_PORT "5150"

// Client structure
struct ClientInfo {
	SOCKET socket;

	// Buffer information (this is basically your buffer class)
	WSABUF dataBuf;
	char buffer[DEFAULT_BUFLEN];
	int bytesRECV;
};

int TotalClients = 0;
ClientInfo* ClientArray[FD_SETSIZE];

void RemoveClient(int index)
{
	ClientInfo* client = ClientArray[index];
	closesocket(client->socket);
	printf("Closing socket %d\n", (int)client->socket);

	for (int clientIndex = index; clientIndex < TotalClients; clientIndex++)
	{
		ClientArray[clientIndex] = ClientArray[clientIndex + 1];
	}

	TotalClients--;

	// We also need to cleanup the ClientInfo data
	// TODO: Delete Client
}

int main(int argc, char** argv)
{
	// This structure will be populated by WSAStartup
	WSADATA wsaData;
	int result;

	// Initialize Winsock
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		printf("Winsock initialization failed with error: %d\n", result);
		return 1;
	}
	else
	{
		printf("WSAStartup() was successful!\n");
	}

	// #1 Socket
	SOCKET listenSocket = INVALID_SOCKET;
	SOCKET acceptSocket = INVALID_SOCKET;

	struct addrinfo* addrResult = NULL;
	struct addrinfo hints;

	// Define our connection address info 
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	result = getaddrinfo(NULL, DEFAULT_PORT, &hints, &addrResult);
	if (result != 0)
	{
		printf("getaddrinfo() failed with error %d\n", result);
		WSACleanup();
		return 1;
	}
	else
	{
		printf("getaddrinfo() is good!\n");
	}

	// Create a SOCKET for connecting to the server
	listenSocket = socket(
		addrResult->ai_family,
		addrResult->ai_socktype,
		addrResult->ai_protocol
	);
	if (listenSocket == INVALID_SOCKET)
	{
		printf("socket() failed with error %d\n", WSAGetLastError());
		freeaddrinfo(addrResult);
		WSACleanup();
		return 1;
	}
	else
	{
		printf("socket() is created!\n");
	}

	// #2 Bind - Setup the TCP listening socket
	result = bind(listenSocket, addrResult->ai_addr, (int)addrResult->ai_addrlen);
	if (result == SOCKET_ERROR)
	{
		printf("bind() failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(addrResult);
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}
	else
	{
		printf("bind() is good!\n");
	}

	// We don't need this anymore
	freeaddrinfo(addrResult);

	// #3 Listen
	result = listen(listenSocket, SOMAXCONN);
	if (result == SOCKET_ERROR)
	{
		printf("listen() failed with error %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}
	else
	{
		printf("listen() was successful!\n");
	}
	// we wanna be able to accept multiple clients
	// Change the socket mode on the listening socket from blocking to
	// non-blocking so the application will not block waiting for requests
	DWORD NonBlock = 1;
	// this means when youre doing send requests or receive requests, on this socket, it will not block
	// if theres no data there for it, its just gonna return an error
	result = ioctlsocket(listenSocket, FIONBIO, &NonBlock); //input output control socket
	if (result == SOCKET_ERROR)
	{
		printf("ioctlsocket() failed with error %d\n", WSAGetLastError());
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}
	printf("ioctlsocket() was successful!\n");

	FD_SET ReadSet;
	int total;
	DWORD flags;
	DWORD RecvBytes;
	DWORD SentBytes;

	printf("Entering accept/recv/send loop...\n");
	while (true)
	{
		timeval tv = { 0 };
		tv.tv_sec = 2;
		// Initialize our read set
		FD_ZERO(&ReadSet); // everyloop we initialize to zero

		// Always look for connection attempts
		// we need to read from this socket every loop
		FD_SET(listenSocket, &ReadSet); // set listen socket into the readset which means we're reading from the listensocket

		// every connected client we have, we're gonna add to the readset as well
		// Set read notification for each socket.
		for (int i = 0; i < TotalClients; i++)
		{
			FD_SET(ClientArray[i]->socket, &ReadSet);
		}

		// Call our select function to find the sockets that
		// require our attention
		printf("Waiting for select()...\n");
		total = select(0, &ReadSet, NULL, NULL, &tv); // this is our only blocking call rn, select can wait up to 2 seconds
		// we're getting all the clients all the sockets we have, and marking them as if we need information or not, if they have data for us or not
		if (total == SOCKET_ERROR)
		{
			printf("select() failed with error: %d\n", WSAGetLastError());
			return 1;
		}
		else
		{
			printf("select() is successful!\n");
		}

		// #4 Check for arriving connections on the listening socket
		if (FD_ISSET(listenSocket, &ReadSet)) // is listen socket set inside of it
		{
			total--; // remove one from the total (total is the size of the readset)
			acceptSocket = accept(listenSocket, NULL, NULL);
			if (acceptSocket == INVALID_SOCKET)
			{
				printf("accept() failed with error %d\n", WSAGetLastError());
				return 1;
			}
			else
			{
				result = ioctlsocket(acceptSocket, FIONBIO, &NonBlock); // set the client to be inputoutput control non blocking
				if (result == SOCKET_ERROR)
				{
					printf("ioctsocket() failed with error %d\n", WSAGetLastError());
				}
				else
				{
					printf("ioctlsocket() success!\n");


					ClientInfo* info = new ClientInfo(); // add as client
					info->socket = acceptSocket; // storing new socket as accept socket
					info->bytesRECV = 0; // the write index
					ClientArray[TotalClients] = info; // num of clients we can store
					TotalClients++;
					printf("New client connected on socket %d\n", (int)acceptSocket);
				}
			}
		}

		// after we're done accepting our new client, we can go through all our currently connected
		// clients and we're gonna see if any of them are inside the readset, if they are, we can 
		// call a recieve command from them

		// since receive is a blocking call, if we know we have data ready to read on those sockets, 
		// that it wont be blocking waiting for data to come

		// so for each of our connected clients, 
		// #5 recv & send
		for (int i = 0; i < TotalClients; i++)
		{
			ClientInfo* client = ClientArray[i];

			// check if the socket is set withing the readset

			// If the ReadSet is marked for this socket, then this means data
			// is available to be read on the socket
			if (FD_ISSET(client->socket, &ReadSet))
			{
				// if it is set, we can read the data from that socket
				total--;
				client->dataBuf.buf = client->buffer;
				client->dataBuf.len = DEFAULT_BUFLEN;

				DWORD Flags = 0;
				result = WSARecv( // call a receive call (alternative to just calling receive)
					client->socket,
					&(client->dataBuf),
					1,
					&RecvBytes,
					&Flags,
					NULL,
					NULL
				);


				// once we recieve the data we can do our deserialization
				std::string received(client->dataBuf.buf);
				// buffer.WriteString(received);
				// packetLength = buffer.ReadUInt32LE();

				int value = 0;
				value |= client->dataBuf.buf[0] << 24;
				value |= client->dataBuf.buf[1] << 16;
				value |= client->dataBuf.buf[2] << 8;
				value |= client->dataBuf.buf[3];

				printf("The value received is: %d\n", value);

				std::cout << "RECVd: " << received << std::endl;

				//the result we receive from WSAReceive, remember is result >0 we receive data, if 0 connection lost, 
				if (result == SOCKET_ERROR) // this error may not be a bad error since we are a non-blocking connection
				{
					// it may be a known error called WSAEWOULDBLOCK
					// so normally, WSAReceive would block, and this is the error
					// its just returning bc we're telling it not to block
					if (WSAGetLastError() == WSAEWOULDBLOCK)
					{
						// this error would pop up if c3 (see lecture notes) is not there
						// bc it wouldnt get a result >0 bc that would mean data was receieved which it isnt
						// and it wouldnt get 0 bc the connection wouldnt be lost, so it would get a negative result
						// and cause this error
						// We can ignore this, it isn't an actual error.
					}
					else
					{
						printf("WSARecv failed on socket %d with error: %d\n", (int)client->socket, WSAGetLastError());
						RemoveClient(i);
					}
				}
				else
				{
					printf("WSARecv() is OK!\n");
					if (RecvBytes == 0)
					{
						RemoveClient(i);
					}
					else if (RecvBytes == SOCKET_ERROR)
					{
						printf("recv: There was an error..%d\n", WSAGetLastError());
						continue;
					}
					else
					{
						// RecvBytes > 0, we got data
						result = WSASend( //sending data right back to client
							client->socket,
							&(client->dataBuf),
							1,
							&SentBytes,
							Flags,
							NULL,
							NULL
						);

						// Example using send instead of WSASend...
						//int iSendResult = send(client->socket, client->dataBuf.buf, iResult, 0);

						if (SentBytes == SOCKET_ERROR)
						{
							printf("send error %d\n", WSAGetLastError());
						}
						else if (SentBytes == 0)
						{
							printf("Send result is 0\n"); //(disconnected?)
						}
						else
						{
							printf("Successfully sent %d bytes!\n", SentBytes);
						}
					}
				}
			}
		}

	}
	// #6 close
	result = shutdown(acceptSocket, SD_SEND);
	if (result == SOCKET_ERROR) {
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(acceptSocket);
		WSACleanup();
		return 1;
	}

	// cleanup
	closesocket(acceptSocket);
	WSACleanup();

	return 0;
}