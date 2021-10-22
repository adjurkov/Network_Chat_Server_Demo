#pragma once
#include <string>


enum class MessageType
{
	// Client
	JoinRoom,
	LeaveRoom,
	SendMessage,
	ReceiveMessage
};


struct sHeader
{
	int packet_length;
	int message_id;
	
};

///int username_length;
	//std::string username;

