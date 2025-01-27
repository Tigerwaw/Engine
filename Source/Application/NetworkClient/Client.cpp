#include "Client.h"
#include <iostream>
#include <WinSock2.h>

#include "NetMessages/NetMessage_Text.h"
#include "NetMessages/NetMessage_Connect.h"
#include "NetMessages/NetMessage_Disconnect.h"
#include "NetMessages/NetMessage_RequestHandshake.h"
#include "NetMessages/NetMessage_AcceptHandshake.h"

Client::Client()
{
    myComm.Init(false, false);
    myReceiveThread = std::thread(&Client::Receive, this);
}

Client::~Client()
{
    myReceiveThread.join();
    myComm.Destroy();
}

void Client::SendInput(std::string aMessage)
{
    if (!myHasEstablishedConnection)
    {
        SendConnectMessage(aMessage);
        myHasEstablishedConnection = true;
    }
    else
    {
        if (aMessage == "quit")
        {
            SendDisconnectMessage();
        }
        else
        {
            SendTextMessage(aMessage);
        }
    }
}

void Client::Receive()
{
    if (!myHasEstablishedHandshake)
    {
        SendHandshakeRequest();
    }

    sockaddr_in otherAddress = {};
    NetBuffer receiveBuffer;
    if (int bytesReceived = myComm.ReceiveData(receiveBuffer, otherAddress); bytesReceived > 0)
    {
        NetMessage* receivedMessage = ReceiveMessage(receiveBuffer);

        if (receivedMessage)
        {
            receivedMessage->Deserialize(receiveBuffer);
            HandleMessage(receivedMessage);
            delete(receivedMessage);
        }
    }
}

void Client::SendHandshakeRequest() const
{
    NetMessage_RequestHandshake msg;
    NetBuffer sendBuffer;
    msg.Serialize(sendBuffer);
    myComm.SendData(sendBuffer, myComm.GetAddress());
}

void Client::SendTextMessage(const std::string& aMessage) const
{
    NetMessage_Text textMsg;
    textMsg.SetData(aMessage);
    NetBuffer sendBuffer;
    textMsg.Serialize(sendBuffer);
    myComm.SendData(sendBuffer, myComm.GetAddress());
}

void Client::SendConnectMessage(const std::string& aUsername) const
{
    NetMessage_Connect connectMsg;
    connectMsg.SetData(aUsername);
    NetBuffer sendBuffer;
    connectMsg.Serialize(sendBuffer);
    myComm.SendData(sendBuffer, myComm.GetAddress());
}

void Client::SendDisconnectMessage() const
{
    NetMessage_Disconnect disconnectMsg;
    NetBuffer sendBuffer;
    disconnectMsg.Serialize(sendBuffer);
    myComm.SendData(sendBuffer, myComm.GetAddress());
}

NetMessage* Client::ReceiveMessage(const NetBuffer& aBuffer) const
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
    case NetMessageType::HandshakeAccept:
    {
        return new NetMessage_AcceptHandshake();
        break;
    }
    default:
        return nullptr;
        break;
    }
}

void Client::HandleMessage(NetMessage* aMessage)
{
    NetMessageType type = aMessage->GetType();
    switch (type)
    {
    case NetMessageType::Connect:
        HandleMessage_Connect(*static_cast<NetMessage_Connect*>(aMessage));
        break;
    case NetMessageType::Disconnect:
        HandleMessage_Disconnect(*static_cast<NetMessage_Disconnect*>(aMessage));
        break;
    case NetMessageType::Text:
        HandleMessage_Text(*static_cast<NetMessage_Text*>(aMessage));
        break;
    case NetMessageType::HandshakeRequest:
        break;
    case NetMessageType::HandshakeAccept:
    {
        if (!myHasEstablishedHandshake)
        {
            printf("Enter your username: ");
            myHasEstablishedHandshake = true;
        }
        break;
    }
    default:
        break;
    }
}

void Client::HandleMessage_Connect(NetMessage_Connect& aMessage)
{
    printf("[%s] has joined the chat!\n", aMessage.GetData().data());
}

void Client::HandleMessage_Disconnect(NetMessage_Disconnect& aMessage)
{
    printf("[%s] has left the chat.\n", aMessage.GetData().data());
}

void Client::HandleMessage_Text(NetMessage_Text& aMessage)
{
    printf("%s\n", aMessage.GetData().data());
}

void Client::HandleMessage_HandshakeAccept()
{
    myHasEstablishedHandshake = true;
}
