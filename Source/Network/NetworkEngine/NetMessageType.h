#pragma once

enum class NetMessageType
{
	None,
	RequestHandshake,
	AcceptHandshake,
	RequestConnect,
	AcceptConnect,
	Disconnect,
	Text,
	CreateCharacter,
	RemoveCharacter,
	Position,
	Test
};