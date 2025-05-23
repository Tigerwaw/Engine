#pragma once

enum class NetMessageType
{
	None,
	HandshakeRequest,
	HandshakeAccept,
	Connect,
	ConnectAccept,
	Disconnect,
	Text,
	CreateCharacter,
	RemoveCharacter,
	Position,
	Test
};