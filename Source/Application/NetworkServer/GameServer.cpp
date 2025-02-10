#include <Enginepch.h>
#include "GameServer.h"
#include <iostream>

#include "NetworkEngine/NetMessage.h"
#include "NetworkShared/NetMessages/NetMessage_Text.h"
#include "NetworkShared/NetMessages/NetMessage_Connect.h"
#include "NetworkShared/NetMessages/NetMessage_Disconnect.h"
#include "NetworkShared/NetMessages/NetMessage_RequestHandshake.h"
#include "NetworkShared/NetMessages/NetMessage_AcceptHandshake.h"
#include "NetworkShared/NetMessages/NetMessage_CreateCharacter.h"
#include "NetworkShared/NetMessages/NetMessage_Position.h"

#include <GameEngine/Engine.h>
#include <Time/Timer.h>

void GameServer::Update()
{
    //float dt = Engine::GetInstance().GetTimer().GetDeltaTime();
    //if (dt > (1.0f / 60.0f))
    //{
    //    // Send queued message.
    //}
}

NetMessage* GameServer::ReceiveMessage(const NetBuffer& aBuffer) const
{
    NetMessageType receivedMessageType = static_cast<NetMessageType>(aBuffer.GetBuffer()[0]);

    switch (receivedMessageType)
    {
    case NetMessageType::Connect:
    {
        return new NetMessage_Connect();
        break;
    }
    case NetMessageType::Disconnect:
    {
        return new NetMessage_Disconnect();
        break;
    }
    case NetMessageType::Text:
    {
        return new NetMessage_Text();
        break;
    }
    case NetMessageType::HandshakeRequest:
    {
        return new NetMessage_RequestHandshake();
        break;
    }
    case NetMessageType::Position:
    {
        return new NetMessage_Position();
        break;
    }
    default:
        return nullptr;
        break;
    }
}

void GameServer::HandleMessage(NetMessage* aMessage, const sockaddr_in& aAddress, const int aBytesReceived)
{
    NetMessageType type = aMessage->GetType();
    switch (type)
    {
    case NetMessageType::Connect:
        HandleMessage_Connect(*static_cast<NetMessage_Connect*>(aMessage), aAddress);
        break;
    case NetMessageType::Disconnect:
        HandleMessage_Disconnect(*static_cast<NetMessage_Disconnect*>(aMessage), aAddress);
        break;
    case NetMessageType::Text:
        HandleMessage_Text(*static_cast<NetMessage_Text*>(aMessage), aAddress, aBytesReceived);
        break;
    case NetMessageType::HandshakeRequest:
        HandleMessage_HandshakeRequest(aAddress);
        break;
    case NetMessageType::HandshakeAccept:
        break;
    case NetMessageType::Position:
        HandleMessage_Position(*static_cast<NetMessage_Position*>(aMessage), aAddress);
        break;
    default:
        break;
    }
}

void GameServer::HandleMessage_Connect(NetMessage_Connect& aMessage, const sockaddr_in& aAddress)
{
    if (DoesClientExist(aAddress)) return;

    NetInfo& newInfo = myClients.emplace_back();
    newInfo.address = aAddress;
    newInfo.username = aMessage.GetUsername();
    newInfo.id = myCurrentNetworkID;
    myCurrentNetworkID++;
    int index = GetClientIndex(newInfo.address);

    printf("\n[User %s connected]", newInfo.username.data());

    // Send connect message.
    {
        NetMessage_Connect connectMsg;
        connectMsg.SetNetworkID(newInfo.id);
        connectMsg.SetUsername(newInfo.username);
        NetBuffer buffer;
        connectMsg.Serialize(buffer);
        SendToAllClients(buffer);
    }

    CU::Vector3f startingPos;
    startingPos.x = static_cast<float>((std::rand() % 1000) - 500);
    startingPos.y = 0.0f;
    startingPos.z = static_cast<float>((std::rand() % 1000) - 500);

    // Create character for newly joined user.
    {
        NetMessage_CreateCharacter createCharacterMsg;
        createCharacterMsg.SetNetworkID(newInfo.id);
        createCharacterMsg.SetIsClient(true);
        createCharacterMsg.SetStartingPosition(startingPos);
        NetBuffer buffer;
        createCharacterMsg.Serialize(buffer);
        myComm.SendData(buffer, myClients[index].address);
    }

    // Create character for all other clients.
    {
        NetMessage_CreateCharacter createCharacterMsg;
        createCharacterMsg.SetNetworkID(newInfo.id);
        createCharacterMsg.SetIsClient(false);
        createCharacterMsg.SetStartingPosition(startingPos);
        NetBuffer buffer;
        createCharacterMsg.Serialize(buffer);
        SendToAllClientsExcluding(buffer, index);
    }

    // Create already existing client characters for newly joined user.
    for (auto& client : myClients)
    {
        if (client.id == newInfo.id) continue;

        NetMessage_CreateCharacter createCharacterMsg;
        createCharacterMsg.SetNetworkID(client.id);
        createCharacterMsg.SetIsClient(false);
        NetBuffer buffer;
        createCharacterMsg.Serialize(buffer);
        myComm.SendData(buffer, myClients[index].address);
    }
}

void GameServer::HandleMessage_Disconnect(NetMessage_Disconnect&, const sockaddr_in& aAddress)
{
    if (!DoesClientExist(aAddress)) return;

    int index = GetClientIndex(aAddress);
    std::string username = myClients[index].username;
    printf("\n[User %s disconnected]", username.data());
    
    myClients.erase(myClients.begin() + index);

    NetMessage_Disconnect disconnectMsg;
    disconnectMsg.SetData(username);
    NetBuffer buffer;
    disconnectMsg.Serialize(buffer);
    SendToAllClients(buffer);
}

void GameServer::HandleMessage_Text(NetMessage_Text& aMessage, const sockaddr_in& aAddress, const int aBytesReceived)
{
    if (!DoesClientExist(aAddress)) return;

    int index = GetClientIndex(aAddress);

    std::string data = aMessage.GetData();
    printf("\n[User Message] [%s]: %s [Bytes: %i]", myClients[index].username.data(), data.data(), aBytesReceived);

    NetMessage_Text newMsg;
    newMsg.SetData("\n[" + myClients[index].username + "]" + ": " + data);
    NetBuffer buffer;
    newMsg.Serialize(buffer);
    SendToAllClients(buffer);
}

void GameServer::HandleMessage_Position(NetMessage_Position& aMessage, const sockaddr_in& aAddress)
{
    if (!DoesClientExist(aAddress)) return;

    NetMessage_Position newMsg;
    unsigned clientID = myClients[GetClientIndex(aAddress)].id;
    newMsg.SetNetworkID(clientID);
    CU::Vector3f pos = aMessage.GetPosition();
    newMsg.SetPosition(pos);
    NetBuffer buffer;
    newMsg.Serialize(buffer);
    SendToAllClients(buffer);

    printf("\n Received position x: %f, y: %f, z: %f, from ID %i", pos.x, pos.y, pos.z, clientID);
}

void GameServer::HandleMessage_HandshakeRequest(const sockaddr_in& aAddress)
{
    if (DoesClientExist(aAddress)) return;

    NetMessage_AcceptHandshake msg;
    NetBuffer buffer;
    msg.Serialize(buffer);
    myComm.SendData(buffer, aAddress);
    printf("\nAccepted handshake for adress [%i] : [%i]", aAddress.sin_addr.S_un.S_addr, aAddress.sin_port);
}