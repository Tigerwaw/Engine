#pragma once

enum class NetMessageType
{
	HandshakeRequest,
	HandshakeAccept,
	Connect,
	Disconnect,
	Text
};