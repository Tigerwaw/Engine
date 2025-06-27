#pragma once
#define DEBUG_SENT_MESSAGE_TYPE
#define DEBUG_RECEIVED_MESSAGE_TYPE
#define DEBUG_CREATE_GUARANTEED_MESSAGE
#define DEBUG_SEND_GUARANTEED_MESSAGE
#define DEBUG_ACKNOWLEDGED_MESSAGE
#define DEBUG_ACCEPT_HANDSHAKE
#define DEBUG_ADD_CLIENT
#define DEBUG_REMOVE_CLIENT
#define DEBUG_CLIENT_DISCONNECTED
#define DEBUG_ATTEMPTING_CONNECT
#define DEBUG_ATTEMPTING_DISCONNECT
//#define DEBUG_SEND_POSITION
#define DEBUG_ESTABLISHED_HANDSHAKE
#define DEBUG_ESTABLISHED_CONNECTION

namespace NetworkDefines
{
	namespace Client
	{
		constexpr float tickRate = 60.0f;
		constexpr int maxMessagesHandledPerTick = 50;
		constexpr float playerPositionSendTickRate = 10.0f;
		constexpr float pingServerRate = 1.0f;
		constexpr float handshakeRequestRate = 0.5f;
		constexpr float connectRequestRate = 0.5f;
	}

	namespace Server
	{
		constexpr float tickRate = 10.0f;
		constexpr int maxMessagesHandledPerTick = 50;
		constexpr float pingClientsRate = 1.0f;
	}

	constexpr int defaultBufferSize = 512;
	constexpr float guaranteedMessageTimeout = 1.0f;
	constexpr int guaranteedMesssageMaxTimeouts = 3;
	constexpr float networkStatsUpdateRate = 1.0f;
}