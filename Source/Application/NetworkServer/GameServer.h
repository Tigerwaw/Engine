#pragma once
#include "ServerBase.h"

class NetMessage_Connect;
class NetMessage_AcceptConnect;
class NetMessage_Disconnect;
class NetMessage_Text;
class NetMessage_CreateCharacter;
class NetMessage_Position;

class GameServer : public ServerBase
{
public:
    void Update() override;
protected:
    NetMessage* ReceiveMessage(const NetBuffer& aBuffer) const override;
    void HandleMessage(NetMessage* aMessage, const sockaddr_in& aAddress, const int aBytesReceived) override;

    void HandleMessage_Connect(NetMessage_Connect& aMessage, const sockaddr_in& aAddress);
    void HandleMessage_Disconnect(NetMessage_Disconnect& aMessage, const sockaddr_in& aAddress);

    void HandleMessage_HandshakeRequest(const sockaddr_in& aAddress);

    void CreateNewObject();
    void DestroyObject(unsigned aNetworkID);
    void UpdatePositions();

    void SendTestMessage();

private:
    unsigned myCurrentNetworkID = 1;
    std::vector<std::shared_ptr<GameObject>> myObjects;

    float myTickRate = 10.0f;

    int myCurrentlyActiveObjects = 0;
    int myObjectLimit = 16;

    double myLastUpdateTimestamp = 0;
    float myTimeBetweenObjectsSpawned = 1.0f;
    float myCurrentTimeSinceLastSpawn = 0;
};

