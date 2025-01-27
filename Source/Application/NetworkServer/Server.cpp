#include "Server.h"
#include <iostream>

#include "NetMessages/NetMessage_Text.h"
#include "NetMessages/NetMessage_Connect.h"
#include "NetMessages/NetMessage_Disconnect.h"
#include "NetMessages/NetMessage_RequestHandshake.h"
#include "NetMessages/NetMessage_AcceptHandshake.h"

Server::Server()
{
    myComm.Init(true, true);
    myReceiveThread = std::thread(&Server::Receive, this);
}

Server::~Server()
{
    myReceiveThread.join();
    myComm.Destroy();
}

void Server::Receive()
{
    sockaddr_in otherAddress = {};
    NetBuffer receiveBuffer;
    if (int bytesReceived = myComm.ReceiveData(receiveBuffer, otherAddress); bytesReceived > 0)
    {
        NetMessage* receivedMessage = ReceiveMessage(receiveBuffer);

        if (receivedMessage)
        {
            receivedMessage->Deserialize(receiveBuffer);
            HandleMessage(receivedMessage, otherAddress, bytesReceived);
            delete(receivedMessage);
        }
    }
}

NetMessage* Server::ReceiveMessage(const NetBuffer& aBuffer) const
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

void Server::HandleMessage(NetMessage* aMessage, const sockaddr_in& aAddress, const int aBytesReceived)
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
    default:
        break;
    }
}

void Server::HandleMessage_Connect(NetMessage_Connect& aMessage, const sockaddr_in& aAddress)
{
    if (DoesClientExist(aAddress)) return;

    NetInfo& newInfo = myClients.emplace_back();
    newInfo.address = aAddress;
    newInfo.username = aMessage.GetData();

    printf("[User %s connected]\n", newInfo.username.data());

    NetMessage_Connect connectMsg;
    connectMsg.SetData(newInfo.username);
    NetBuffer buffer;
    connectMsg.Serialize(buffer);
    SendToAllClients(buffer);
}

void Server::HandleMessage_Disconnect(NetMessage_Disconnect&, const sockaddr_in& aAddress)
{
    if (!DoesClientExist(aAddress)) return;

    int index = GetClientIndex(aAddress);
    std::string username = myClients[index].username;
    printf("[User %s disconnected]\n", username.data());
    
    myClients.erase(myClients.begin() + index);

    NetMessage_Disconnect disconnectMsg;
    disconnectMsg.SetData(username);
    NetBuffer buffer;
    disconnectMsg.Serialize(buffer);
    SendToAllClients(buffer);
}

void Server::HandleMessage_Text(NetMessage_Text& aMessage, const sockaddr_in& aAddress, const int aBytesReceived)
{
    if (!DoesClientExist(aAddress)) return;

    int index = GetClientIndex(aAddress);

    std::string data = aMessage.GetData();
    printf("[User Message] [%s]: %s [Bytes: %i]\n", myClients[index].username.data(), data.data(), aBytesReceived);

    NetMessage_Text newMsg;
    newMsg.SetData("[" + myClients[index].username + "]" + ": " + data + "\n");
    NetBuffer buffer;
    newMsg.Serialize(buffer);
    SendToAllClients(buffer);
}

void Server::HandleMessage_HandshakeRequest(const sockaddr_in& aAddress)
{
    if (DoesClientExist(aAddress)) return;

    NetMessage_AcceptHandshake msg;
    NetBuffer buffer;
    msg.Serialize(buffer);
    myComm.SendData(buffer, aAddress);
    printf("Accepted handshake for adress [%i] : [%i]\n", aAddress.sin_addr.S_un.S_addr, aAddress.sin_port);
}

void Server::SendToAllClients(NetBuffer& aBuffer) const
{
    for (auto& client : myClients)
    {
        myComm.SendData(aBuffer, client.address);
    }
}

bool Server::DoesClientExist(const sockaddr_in& aAddress) const
{
    if (myClients.empty()) return false;

    for (int i = 0; i < myClients.size(); i++)
    {
        if (myClients[i] == aAddress)
        {
            return true;
        }
    }

    return false;
}

const int Server::GetClientIndex(const sockaddr_in& aAddress) const
{
    for (int i = 0; i < myClients.size(); i++)
    {
        if (myClients[i] == aAddress)
        {
            return i;
        }
    }

    return -1;
}