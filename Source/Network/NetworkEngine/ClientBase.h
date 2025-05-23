#pragma once
#include <string>
#include <thread>
#include <chrono>

#include "Communicator.h"

class NetMessage;

class ClientBase
{
public:
	virtual ~ClientBase();
	void ConnectClient(const char* aIP);

	virtual void Update();
protected:
	void Receive();
	virtual void SendHandshakeRequest() const = 0;
	virtual void SendConnectionRequest(const std::string& aUsername) const = 0;
	virtual void SendDisconnectMessage() const = 0;
	virtual void HandleMessage_AcceptHandshake();
	virtual void HandleMessage_AcceptConnect();

	virtual NetMessage* ReceiveMessage(const NetBuffer& aBuffer) const = 0;
	virtual void HandleMessage(NetMessage* aMessage) = 0;

	bool HasEstablishedHandshake() const { return myHasEstablishedHandshake; }
	bool HasEstablishedConnection() const { return myHasEstablishedConnection; }

	Communicator myComm;
	bool myHasEstablishedHandshake = false;
	bool myHasEstablishedConnection = false;

	bool myShouldReceive = false;

	std::chrono::system_clock::time_point myLastHandshakeRequestTime;
	float myTimeBetweenHandshakeRequests = 2.0f;
	int myMessagesHandledPerTick = 10;
};

