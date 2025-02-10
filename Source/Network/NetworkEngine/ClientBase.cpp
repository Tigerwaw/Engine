#include "ClientBase.h"

#include "NetMessage.h"
#include <WinSock2.h>

ClientBase::ClientBase()
{
}

ClientBase::~ClientBase()
{
    myComm.Destroy();
}

void ClientBase::Update()
{
    if (myShouldReceive)
    {
        Receive();
    }
}

void ClientBase::StartReceive(const char* aIP)
{
    myComm.Init(false, false, aIP);
    SendHandshakeRequest();
    myLastHandshakeRequestTime = std::chrono::system_clock::now();
    myShouldReceive = true;
}

void ClientBase::Receive()
{
    if (!myHasEstablishedHandshake)
    {
        std::chrono::duration<float> elapsed_seconds = std::chrono::system_clock::now() - myLastHandshakeRequestTime;
        if (elapsed_seconds.count() > myTimeBetweenHandshakeRequests)
        {
            myLastHandshakeRequestTime = std::chrono::system_clock::now();
            SendHandshakeRequest();
        }
    }

    sockaddr_in otherAddress = {};
    NetBuffer receiveBuffer;
    if (int bytesReceived = myComm.ReceiveData(receiveBuffer, otherAddress); bytesReceived > 0)
    {
        NetMessage* receivedMessage = nullptr;
        receivedMessage = ReceiveMessage(receiveBuffer);

        if (receivedMessage)
        {
            receivedMessage->Deserialize(receiveBuffer);
            HandleMessage(receivedMessage);
            delete receivedMessage;
        }
    }
}

void ClientBase::HandleMessage_HandshakeAccept()
{
    myHasEstablishedHandshake = true;
}