#pragma once
#include <string>

// Compiled as a dynamic library
#define DLLExport __declspec ( dllexport )

enum
{
	// Client
	JoinRoom = 1,
	LeaveRoom = 2,
	SetName = 3,
	Broadcast = 4,
};

struct DLLExport sPacket
{

	struct sHeader
	{
		int packetLength;
		int msgID;
	};

	sHeader header;

	int roomLength;
	std::string roomname;

	int usernameLength;
	std::string username;

	int msgLength;
	std::string msg;

	sPacket()
	{
		header.msgID = 4;
		header.packetLength = 0;
		roomLength = 0;
		roomname = " ";
		usernameLength = 0;
		username = " ";
		msgLength = 0;
		msg = " ";
	}

};


//class DLLExport cProtocolManager
//{
//	
//
//	struct sHeader
//	{
//		int packet_length;
//		int message_id;
//	};
//
//	sHeader header;
//	int username_length;
//	std::string username;
//	int room_length;
//	std::string room_name;
//	int user_message_length;
//	std::string user_message;
//
//
//	//struct sJoinRoomMessage
//	//{
//	//	sHeader header;
//	//	int room_length;
//	//	std::string room_name;
//	//};
//
//	//struct sLeaveRoomMessage
//	//{
//	//	sHeader header;
//	//	int room_length;
//	//	std::string room_name;
//	//};
//
//	//struct sSendMessage
//	//{
//	//	sHeader header;
//	//	int room_length;
//	//	std::string room_name;
//	//	int user_message_length;
//	//	std::string user_message;
//	//};
//
//	//struct sReceiveMessage
//	//{
//	//	sHeader header;
//	//	int username_length;
//	//	std::string username;
//	//	int user_message_length;
//	//	std::string user_message;
//	//	int room_length;
//	//	std::string room_name;
//	//};
//};
//



