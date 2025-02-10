#include "ClientBase.h"

#include "NetMessage.h"
#include <WinSock2.h>

ClientBase::ClientBase()
{
}

ClientBase::~ClientBase()
{
    myReceiveThread.join();
    myComm.Destroy();
}

void ClientBase::Update()
{
}

void ClientBase::StartReceive(ClientBase* aClient, const char* aIP)
{
    myComm.Init(false, false, aIP);
    myReceiveThread = std::thread(&ClientBase::Receive, aClient);
    SendHandshakeRequest();
    myLastHandshakeRequestTime = std::chrono::system_clock::now();
}

void ClientBase::Receive()
{
    while (myShouldReceive)
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
}

void ClientBase::HandleMessage_HandshakeAccept()
{
    myHasEstablishedHandshake = true;
}