#pragma once

enum class NetMessageType
{
	None,
	HandshakeRequest,
	HandshakeAccept,
	Connect,
	Disconnect,
	Text,
	CreateCharacter,
	RemoveCharacter,
	Position,
	Test
};