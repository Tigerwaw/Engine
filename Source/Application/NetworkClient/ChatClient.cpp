#include "ChatClient.h"
#include <iostream>
#include <WinSock2.h>

#include "NetworkEngine/NetMessage.h"
#include "NetworkShared/NetMessages/NetMessage_Text.h"
#include "NetworkShared/NetMessages/NetMessage_Connect.h"
#include "NetworkShared/NetMessages/NetMessage_Disconnect.h"
#include "NetworkShared/NetMessages/NetMessage_RequestHandshake.h"
#include "NetworkShared/NetMessages/NetMessage_AcceptHandshake.h"

ChatClient::ChatClient()
{
    StartReceive(this);
    printf("\nWaiting for server...");
}

void ChatClient::SendInput(std::string aMessage)
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


void ChatClient::SendHandshakeRequest() const
{
    NetMessage_RequestHandshake msg;
    NetBuffer sendBuffer;
    msg.Serialize(sendBuffer);
    myComm.SendData(sendBuffer, myComm.GetAddress());
}

void ChatClient::SendTextMessage(const std::string& aMessage) const
{
    NetMessage_Text textMsg;
    textMsg.SetData(aMessage);
    NetBuffer sendBuffer;
    textMsg.Serialize(sendBuffer);
    myComm.SendData(sendBuffer, myComm.GetAddress());
}

void ChatClient::SendConnectMessage(const std::string& aUsername) const
{
    NetMessage_Connect connectMsg;
    connectMsg.SetData(aUsername);
    NetBuffer sendBuffer;
    connectMsg.Serialize(sendBuffer);
    myComm.SendData(sendBuffer, myComm.GetAddress());
}

void ChatClient::SendDisconnectMessage() const
{
    NetMessage_Disconnect disconnectMsg;
    NetBuffer sendBuffer;
    disconnectMsg.Serialize(sendBuffer);
    myComm.SendData(sendBuffer, myComm.GetAddress());
}

NetMessage* ChatClient::ReceiveMessage(const NetBuffer& aBuffer) const
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

void ChatClient::HandleMessage(NetMessage* aMessage)
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
            printf("\nEnter your username: ");
            myHasEstablishedHandshake = true;
        }
        break;
    }
    default:
        break;
    }
}

void ChatClient::HandleMessage_Connect(NetMessage_Connect& aMessage)
{
    printf("\n[%s] has joined the chat!", aMessage.GetData().data());
}

void ChatClient::HandleMessage_Disconnect(NetMessage_Disconnect& aMessage)
{
    printf("\n[%s] has left the chat.", aMessage.GetData().data());
}

void ChatClient::HandleMessage_Text(NetMessage_Text& aMessage)
{
    printf("\n%s", aMessage.GetData().data());
}