#pragma once

enum class NetMessageType
{
	None,
	RTTPing,
	AckMessage,
	RequestHandshake,
	AcceptHandshake,
	RequestConnect,
	AcceptConnect,
	Disconnect,
	Text,
	CreateCharacter,
	RemoveCharacter,
	Position,
	Test,
	UpdateVisibleObjects
};