#pragma once
#include <string>
#include <thread>
#include <chrono>
#include "CommonUtilities/CircularArray.hpp"
#include "Communicator.h"

class NetMessage;
class NetMessage_Connect;
class NetMessage_AcceptConnect;
class NetMessage_Disconnect;
class NetMessage_Text;
class NetMessage_CreateCharacter;
class NetMessage_RemoveCharacter;
class NetMessage_Position;
class NetMessage_Test;

class GameObject;

struct PositionData
{
    Math::Vector3f position;
    double clientTimestamp;
    double serverTimestamp;
};

class GameClient
{
public:
    void ConnectClient(const char* aIP);
    void Update();

    int GetReceivedData() const { return myAvgDataReceived; }
    int GetSentData() const { return myAvgDataSent; }

protected:
    void Receive();
    void Send(const NetBuffer& aNetBuffer);

    NetMessage* ReceiveMessage(const NetBuffer& aBuffer) const;
    void HandleMessage(NetMessage* aMessage);

    void SendHandshakeRequest();
    void SendConnectionRequest(const std::string& aUsername);
    void SendDisconnectMessage();
    void SendTextMessage(const std::string& aMessage);
    void SendPositionMessage(const Math::Vector3f& aPosition);

    void HandleMessage_AcceptHandshake();
    void HandleMessage_AcceptConnect();
    void HandleMessage_Disconnect(NetMessage_Disconnect& aMessage);
    void HandleMessage_Text(NetMessage_Text& aMessage);
    void HandleMessage_CreateCharacter(NetMessage_CreateCharacter& aMessage);
    void HandleMessage_RemoveCharacter(NetMessage_RemoveCharacter& aMessage);
    void HandleMessage_Position(NetMessage_Position& aMessage);
    void HandleMessage_Test(NetMessage_Test& aMessage);

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

    std::unordered_map<unsigned, Utilities::CircularArray<PositionData, 16>> myObjectIDPositionHistory;
    bool myShouldLerpPositions = true;
};