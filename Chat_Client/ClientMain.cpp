#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <iostream>
#include <vector>
#include "ProtocolManager.h"
#include "cBuffer.h"

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_BUFLEN 512						// Default buffer length of our buffer in characters
#define DEFAULT_PORT "5150"	                    // Default port
#define SERVER "127.0.0.1"						// The IP of our server  
sPacket packet;                                 // Packet structure and protocol manager


int main(int argc, char** argv)
{
	int result;									// Results of any commands used
	WSADATA wsaData;							// Contains Winsock data
	SOCKET connectSocket = INVALID_SOCKET;		// Socket used to connect to the server
	struct addrinfo* infoResult = NULL;			// Holds the address information of server
	struct addrinfo* ptr = NULL;
	struct addrinfo hints;

	
//----------------------------------------   Winsock Initialization   ------------------------------------------------

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

	//------------------------------     Resolve Server Address and Port   --------------------------------------------

	result = getaddrinfo(SERVER, DEFAULT_PORT, &hints, &infoResult); // get address info from the server
	if (result != 0)
	{
		printf("getaddrinfo failed with error: %d\n", result);
		WSACleanup();
		return 1;
	}

	//-------------------------------------   Connect to Address Attempt   ---------------------------------------------

	for (ptr = infoResult; ptr != NULL; ptr = ptr->ai_next)
	{
		// Create a Socket for connecting to server
		connectSocket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

		if (connectSocket == INVALID_SOCKET)
		{
			printf("socket failed with error: %ld\n", WSAGetLastError());
			WSACleanup();
			return 1;
		}

		// Connect to server
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



	bool quit = false;
	bool isConnected = false;
	std::vector<char> userMessage;
	cBuffer buffer(4);
	sPacket packet;
	bool nameIsSet = false;

//----------------------------------------   Main Loop Begins ------------------------------------------------
	while (!quit)
	{
		// Key was hit
		if (_kbhit())
		{
			char key = _getch();

			// Escape key pressed
			if (key == 27)
			{
				quit = true;
			}

			// Backspace key pressed
			else if (key == 8)
			{
				// Erase character from screen
				printf("%c", key);
				printf(" ");
				printf("%c", key);

				// Pop character from vector
				userMessage.pop_back();
			}

			// Enter key pressed
			else if (key == 13)
			{
				printf("\n");

				// Parses user input and serializes packet
				packet.SerializeUserCommand(packet, userMessage, buffer);
				
//----------------------------------------   Send Serialized Packet to Server   ------------------------------------------------

				result = send(connectSocket, buffer._buffer.data(), buffer._buffer.size(), 0); 
				if (result == SOCKET_ERROR)
				{
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(connectSocket);
					WSACleanup();
					return 1;
				}
				printf("Bytes Sent: %ld\n", result);
				isConnected = true;

				//// Step #5 shutdown the connection since no more data will be sent
				//result = shutdown(connectSocket, SD_SEND);
				//if (result == SOCKET_ERROR)
				//{
				//	printf("shutdown failed with error: %d\n", WSAGetLastError());
				//	closesocket(connectSocket);
				//	WSACleanup();
				//	return 1;
				//}
			}
			else
			{
				// If any other key pressed, store in userMessage
				printf("%c", key);
				userMessage.push_back(key);
			}
		}

		if (isConnected)
		{
			isConnected = false;

//-------------------------------------  Receive until the peer closes the connection  ------------------------------------------------
		//	do {

				// Temporarily resizing the buffer 
				buffer._buffer.resize(500);

				result = recv(connectSocket, buffer._buffer.data(), buffer._buffer.size(), 0);
				if (result > 0) // echo   
				{
					printf("Bytes received: %d\n", result);

					// Resizing the buffer appropriately 
					buffer._buffer.resize(result);

				// Did we at least get the packet length?
				if (buffer._buffer.size() >= 4)
				{
					int packetLength = buffer.readIntBE();


					// Did we receive the whole buffer?
					if (buffer._buffer.size() == packetLength)
					{
						// HERE I WILL HAVE SWITCH STATEMENTS FOR MSGIDS
						// Deserialize the message, (then reset your buffer?)
						int msgID = buffer.readIntBE();

						switch (msgID)
						{
//******************************************  MESSAGE ID: ACCEPTED USERNAME  **************************************************
							case AcceptedUsername:
							{
								int msgLength = buffer.readIntBE();
								std::string message = buffer.readString(msgLength);

								// Print welcome message
								printf("%s", message.c_str());
								printf("\n");

								buffer._buffer.clear();
								buffer.readIndex = 0;
								buffer.writeIndex = 0;

								break;
							}
//******************************************  MESSAGE ID: JOIN ROOM  **************************************************
							case JoinRoom:
							{
								int msgLength = buffer.readIntBE();
								std::string message = buffer.readString(msgLength);

								// Print welcome message
								printf("%s", message.c_str());
								printf("\n");

								buffer._buffer.clear();
								buffer.readIndex = 0;
								buffer.writeIndex = 0;

								break;
							}
						}

					}
					else
					{
						printf("Didn't receive full message from server.");
					}
				}
				}
				else if (result == 0)
				{
					printf("Connection closed\n");
				}
				else
				{
					printf("recv failed with error: %d\n", WSAGetLastError());
				}


	//		} while (result > 0);

			// check recv
			// int result = recv(...);
			// if(result == SOCKETERROR) // since we're setting to be non-blockin on purpose, its not an error
			//{
			//	if (WSAGetLastError() == 10035)
			//	{
			//		// ignore this error
			//	}
			//	else
			//	{
			//		// tell the user there was an error
			//		// shutdown properly
			//	}
			//}

		}
	}
	
	// Cleanup
	closesocket(connectSocket);
	WSACleanup();

	system("pause");
	return 0;
}