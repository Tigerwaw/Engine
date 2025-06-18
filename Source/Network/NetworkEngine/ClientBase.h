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

	int GetReceivedData() const { return myAvgDataReceived; }
	int GetSentData() const { return myAvgDataSent; }
protected:
	void Receive();
	void Send(const NetBuffer& aNetBuffer);
	virtual void SendHandshakeRequest() = 0;
	virtual void SendConnectionRequest(const std::string& aUsername) = 0;
	virtual void SendDisconnectMessage() = 0;
	virtual void HandleMessage_AcceptHandshake();
	virtual void HandleMessage_AcceptConnect();

	virtual NetMessage* ReceiveMessage(const NetBuffer& aBuffer) const = 0;
	virtual void HandleMessage(NetMessage* aMessage) = 0;

	bool HasEstablishedHandshake() const { return myHasEstablishedHandshake; }
	bool HasEstablishedConnection() const { return myHasEstablishedConnection; }

	bool myHasEstablishedHandshake = false;
	bool myHasEstablishedConnection = false;

	bool myShouldReceive = false;

	std::chrono::system_clock::time_point myLastHandshakeRequestTime;
	float myTimeBetweenHandshakeRequests = 2.0f;
	int myMessagesHandledPerTick = 10;

private:
	Communicator myComm;

	int myDataReceived = 0;
	int myDataSent = 0;
	int myAvgDataReceived = 0;
	int myAvgDataSent = 0;
	std::chrono::system_clock::time_point myLastDataTickTime;
	float myDataTickRate = 1.0f;
};

