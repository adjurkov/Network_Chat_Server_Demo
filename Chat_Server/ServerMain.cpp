#define WIN32_LEAN_AND_MEAN		

#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h> 
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <map>
#include <iostream>
#include "cBuffer.h"
#include "ProtocolManager.h"

#pragma comment (lib, "Ws2_32.lib")
#define DEFAULT_BUFLEN 16
#define DEFAULT_PORT "5150"

//----------------------------------------   Client Structure   ------------------------------------------------
struct ClientInfo 
{
	SOCKET socket;
	std::string username;

	WSABUF dataBuf;
	cBuffer* buffer;
	int bytesRECV;
};
//----------------------------------------   Global Variables   ------------------------------------------------

// Managing client information, and which room they are in
int TotalClients = 0;
std::vector<ClientInfo*> clientSockets;
std::map<std::string, std::vector<ClientInfo*>> clientsInRooms; // key: roomname, value: clientInfo

// Packet Information
sPacket packet;
int packetLength;
int msgID;
int roomLength;
std::string roomname;
int userMsgLength;
std::string userMsgName;
int usernameLength;
std::string username;

//--------------------------------------------------------------------------------------------------------------

void RemoveClient(int index)
{
	//client1.buffer.

	clientSockets.erase(clientSockets.begin() + index);
	//ClientInfo* client = ClientArray[index];

	closesocket(clientSockets.at(index)->socket);
	printf("Closing socket %d\n", (int)clientSockets.at(index)->socket);

	//for (int clientIndex = index; clientIndex < TotalClients; clientIndex++)
	//{
	//	ClientArray[clientIndex] = ClientArray[clientIndex + 1];
	//}

	TotalClients--;

	// We also need to cleanup the ClientInfo data
	// TODO: Delete Client
}

int main(int argc, char** argv)
{
	//----------------------------------------   Initialize Winsock   ------------------------------------------------
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
		printf("Winsock initialization was successful!\n");
	}

	//----------------------------------------   Create Sockets   ------------------------------------------------
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

	//------------------------------------------   Bind Socket   --------------------------------------------------
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

	//-------------------------------------------   Listening    ---------------------------------------------------
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
		printf("Listening for connections...\n");
	}

	DWORD NonBlock = 1;
	result = ioctlsocket(listenSocket, FIONBIO, &NonBlock); 
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

	//----------------------------------------   Network Loop   ------------------------------------------------
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

		// Adding all the client sockets to the readset for each room
	/*	for (std::map<std::string, std::vector<ClientInfo*>>::iterator iter = clientsInRooms.begin(); iter != clientsInRooms.end(); ++iter)
		{
			std::string room = iter->first;
			std::vector<ClientInfo*> clientInfo = iter->second;*/

			//for (int i = 0; i < TotalClients; i++)

		/*}*/

		// Adding all the client sockets to the readset for each room
		for (std::vector<ClientInfo*>::iterator iter = clientSockets.begin(); iter != clientSockets.end(); ++iter)
			FD_SET((*iter)->socket, &ReadSet);

		// Call our select function to find the sockets that require our attention
		printf("Waiting for select()...\n");
		total = select(0, &ReadSet, NULL, NULL, &tv);
		if (total == SOCKET_ERROR)
		{
			printf("select() failed with error: %d\n", WSAGetLastError());
			return 1;
		}
		else
			printf("select() is successful!\n");

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
					info->buffer = new cBuffer;
					//info->buffer->_buffer.resize(1000);

					/*std::string serverLobby = "ServerLobby";*/
					//std::vector<ClientInfo*> clientVec = clientsInRooms[serverLobby];

					clientSockets.push_back(info);
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

//----------------------------------------   Recv and Send   ------------------------------------------------
			for (int i = 0; i < clientSockets.size(); i++)
			{
				ClientInfo* client = clientSockets.at(i);

				// If the ReadSet is marked for this socket, then this means data is available to be read on the socket
				if (FD_ISSET(client->socket, &ReadSet))
				{
					// if it is set, we can read the data from that socket
					total--;

					client->dataBuf.buf = client->buffer->_buffer.data();
					client->dataBuf.len = client->buffer->_buffer.size();

		/*			client->dataBuf.buf = (char*)client->buffer->_buffer.data();
					client->dataBuf.len = client->buffer->_buffer.size();*/

					DWORD Flags = 0;
					result = WSARecv(
						client->socket,
						&(client->dataBuf),
						1,
						&RecvBytes,
						&Flags,
						NULL,
						NULL
					);

					if (result == 0)
					{
						std::cout << " no error occurs and the receive operation has completed immediately" << std::endl;

					}
					if (result == SOCKET_ERROR)
					{
						printf("send failed with error: %d\n", WSAGetLastError());
					}

					// once we recieve the data we can do our deserialization
				//	std::string received(client->dataBuf.buf);
				//	std::cout << "RECVd: " << received << std::endl;

					//client->buffer->writeString(received);
					memcpy(client->buffer->_buffer.data(), client->dataBuf.buf, client->dataBuf.len);

	/*				(char*)client->buffer->_buffer.data() = client->dataBuf.buf;
					client->buffer->_buffer.resize(client->dataBuf.len);*/

					packetLength = client->buffer->readIntBE();
					msgID = client->buffer->readIntBE();

				/*	packetLength = client->buffer->readIntBE();
					msgID = client->buffer->readIntBE();*/


					switch (msgID)
					{
//*************************************  MsgID: SETNAME  *********************************************
					case SetName:
					{
						// Read in the rest of the packet, store username
						packet.usernameLength = client->buffer->readIntBE();
						packet.username = client->buffer->readString();
						client->username = packet.username;

						// Preparing msg back to server
						packet.msg = "SERVER: Welcome, " + client->username + "!";
						packet.msgLength = packet.msg.length();
						packetLength = 4 + 4 + 4 + packet.msgLength;
						packet.header.packetLength = packetLength;
						packet.header.msgID = AcceptedUsername;

						// Clear the buffer
						client->buffer->_buffer.clear();
						client->buffer->readIndex = 0;
						client->buffer->writeIndex = 0;

						// Growing the buffer just enough to deal with packet
						int newBufferSize = packet.header.packetLength + 1;
						client->buffer->_buffer.resize(newBufferSize);

						// Serialize
						client->buffer->writeIntBE(packet.header.packetLength);
						client->buffer->writeIntBE(packet.header.msgID);
						client->buffer->writeIntBE(packet.msgLength);
						client->buffer->writeString(packet.msg);

						// Result > 0 we receive data, if 0 connection lost, 
						if (result == SOCKET_ERROR)
						{
							if (WSAGetLastError() == WSAEWOULDBLOCK) {}
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
								client->dataBuf.buf = client->buffer->_buffer.data();
								client->dataBuf.len = client->buffer->_buffer.size();
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

								if (SentBytes == SOCKET_ERROR)
									printf("send error %d\n", WSAGetLastError());
								else if (SentBytes == 0)
									printf("Send result is 0\n");
								else
									printf("Successfully sent %d bytes!\n", SentBytes);
							}
						}
					}
					//*************************************  MsgID: JOIN ROOM  *********************************************
					case JoinRoom:
					{
						//std::vector<ClientInfo*> clientSockets;
						//// map[string roomname][vector of client info]
						//std::map<std::string, std::vector<ClientInfo*>> clientsInRooms;
					}
					default:
						break;
					}
					//printf("The value received is: %d\n", value);





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