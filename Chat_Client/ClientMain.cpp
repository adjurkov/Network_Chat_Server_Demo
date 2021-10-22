#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512						// Default buffer length of our buffer in characters
//#define DEFAULT_PORT "27015"					// The default port to use
#define DEFAULT_PORT "5150"	
#define SERVER "127.0.0.1"						// The IP of our server  /using home address for example using a server and client on the same computer

int main(int argc, char** argv)
{
	WSADATA wsaData;							// holds Winsock data
	SOCKET connectSocket = INVALID_SOCKET;		// Our connection socket used to connect to the server

	struct addrinfo* infoResult = NULL;			// Holds the address information of our server
	struct addrinfo* ptr = NULL;
	struct addrinfo hints;

	const char* sendbuf = "Hello World!";		// The messsage to send to the server

	char recvbuf[DEFAULT_BUFLEN];				// The maximum buffer size of a message to send
	int result;									// code of the result of any command we use
	int recvbuflen = DEFAULT_BUFLEN;			// The length of the buffer we receive from the server

	// Step #1 Initialize Winsock
	result = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (result != 0)
	{
		printf("Winsock initialization failed with error: %d\n", result);
		return 1;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;

	// Step #2 Resolve the server address and port
	result = getaddrinfo(SERVER, DEFAULT_PORT, &hints, &infoResult); // get address info from the server
	if (result != 0)
	{
		printf("getaddrinfo failed with error: %d\n", result);
		WSACleanup();
		return 1;
	}

	// Step #3 Attempt to connect to an address until one succeeds
	for (ptr = infoResult; ptr != NULL; ptr = ptr->ai_next)
	{
		// Create a SOCKET for connecting to server
		connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

		if (connectSocket == INVALID_SOCKET)
		{
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		// server will wait at "accept" until the client connects
		// Connect to server.
		result = connect(connectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (result == SOCKET_ERROR)
		{
			closesocket(connectSocket);
			connectSocket = INVALID_SOCKET;
			continue;
		}
		break;
	}

	freeaddrinfo(infoResult);

	if (connectSocket == INVALID_SOCKET)
	{
		printf("Unable to connect to server!\n");
		WSACleanup();
		return 1;
	}
	// the server is waiting to receive from client
	// Step #4 Send the message to the server
	result = send(connectSocket, sendbuf, (int)strlen(sendbuf), 0); //passing in server socket  // sending 12 bytes of data over "hello world"
	if (result == SOCKET_ERROR)
	{
		printf("send failed with error: %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}
	printf("Bytes Sent: %ld\n", result);

	// Step #5 shutdown the connection since no more data will be sent
	result = shutdown(connectSocket, SD_SEND);
	if (result == SOCKET_ERROR)
	{
		printf("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(connectSocket);
		WSACleanup();
		return 1;
	}

	// Step #6 Receive until the peer closes the connection
	do {

		result = recv(connectSocket, recvbuf, recvbuflen, 0);
		if (result > 0) // echo   
		{
			printf("Bytes received: %d\n", result);
			printf("Message: %s\n", &recvbuf);
		}
		else if (result == 0)
		{
			printf("Connection closed\n");
		}
		else
		{
			printf("recv failed with error: %d\n", WSAGetLastError());
		}

	} while (result > 0);

	// Step #7 cleanup
	closesocket(connectSocket);
	WSACleanup();

	system("pause");

	return 0;
}