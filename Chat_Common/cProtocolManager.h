#pragma once
#include <string>

// Compiled as a dynamic library
#define DLLExport __declspec ( dllexport )

class DLLExport cProtocolManager
{
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

	sHeader header;
	int username_length;
	std::string username;
	int room_length;
	std::string room_name;
	int user_message_length;
	std::string user_message;


	//struct sJoinRoomMessage
	//{
	//	sHeader header;
	//	int room_length;
	//	std::string room_name;
	//};

	//struct sLeaveRoomMessage
	//{
	//	sHeader header;
	//	int room_length;
	//	std::string room_name;
	//};

	//struct sSendMessage
	//{
	//	sHeader header;
	//	int room_length;
	//	std::string room_name;
	//	int user_message_length;
	//	std::string user_message;
	//};

	//struct sReceiveMessage
	//{
	//	sHeader header;
	//	int username_length;
	//	std::string username;
	//	int user_message_length;
	//	std::string user_message;
	//	int room_length;
	//	std::string room_name;
	//};
};




