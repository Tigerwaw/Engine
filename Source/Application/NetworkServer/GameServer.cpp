#include "GameServer.h"
#include <iostream>

#include "NetworkEngine/NetMessage.h"
#include "NetworkShared/NetMessages/NetMessage_Text.h"
#include "NetworkShared/NetMessages/NetMessage_Connect.h"
#include "NetworkShared/NetMessages/NetMessage_Disconnect.h"
#include "NetworkShared/NetMessages/NetMessage_RequestHandshake.h"
#include "NetworkShared/NetMessages/NetMessage_AcceptHandshake.h"
#include "NetworkShared/NetMessages/NetMessage_CreateCharacter.h"

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
    case NetMessageType::CreateCharacter:
        HandleMessage_CreateCharacter(*static_cast<NetMessage_CreateCharacter*>(aMessage), aAddress);
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
    newInfo.username = aMessage.GetData();

    printf("\n[User %s connected]", newInfo.username.data());

    NetMessage_Connect connectMsg;
    connectMsg.SetData(newInfo.username);
    NetBuffer buffer;
    connectMsg.Serialize(buffer);
    SendToAllClients(buffer);
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

void GameServer::HandleMessage_CreateCharacter(NetMessage_CreateCharacter& aMessage, const sockaddr_in& aAddress)
{
    aMessage;
    aAddress;
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