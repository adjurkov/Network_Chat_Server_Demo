#pragma once
#include <Windows.h>
#include <WinSock2.h>
#include <WS2tcpip.h> 
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <iostream>

// Forward declaration of class
class cTCPServer;

// Callback to data recieved
typedef void* MessageReceived(cTCPServer* listener, int socketID, std::string msg);
class cTCPServer
{
	cTCPServer(std::string ipAddress, int port, MessageReceived msgHandler);

	// 9:40 video 

};