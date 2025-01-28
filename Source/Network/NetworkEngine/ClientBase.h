#pragma once
#include <string>
#include <thread>

#include "Communicator.h"

class NetMessage;

class ClientBase
{
public:
	ClientBase();
	virtual ~ClientBase();
protected:
	// Pass in 'this' from subclass.
	virtual void StartReceive(ClientBase* aClient);
	void Receive();
	virtual void SendHandshakeRequest() const = 0;
	virtual void HandleMessage_HandshakeAccept();

	virtual NetMessage* ReceiveMessage(const NetBuffer& aBuffer) const = 0;
	virtual void HandleMessage(NetMessage* aMessage) = 0;

	bool HasEstablishedHandshake() const { return myHasEstablishedHandshake; }
	bool HasEstablishedConnection() const { return myHasEstablishedConnection; }

	Communicator myComm;
	bool myHasEstablishedHandshake = false;
	bool myHasEstablishedConnection = false;

	std::thread myReceiveThread;
	bool myShouldReceive = true;
};

