#include "ClientBase.h"

#include "NetMessage.h"
#include <WinSock2.h>

ClientBase::~ClientBase()
{
    myComm.Destroy();
}

void ClientBase::ConnectClient(const char* aIP)
{
    myComm.Init(false, false, aIP);
    SendHandshakeRequest();
    myLastHandshakeRequestTime = std::chrono::system_clock::now();
    myShouldReceive = true;
    printf("\nWaiting for server...");
}

void ClientBase::Update()
{
    if (myShouldReceive)
    {
        Receive();
    }
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

    for (int i = 0; i < myMessagesHandledPerTick; i++)
    {
        sockaddr_in otherAddress = {};
        NetBuffer receiveBuffer;
        int bytesReceived = myComm.ReceiveData(receiveBuffer, otherAddress);
        if (bytesReceived > 0)
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
        else
        {
            break;
        }
    }
}

void ClientBase::HandleMessage_AcceptHandshake()
{
    myHasEstablishedHandshake = true;
    printf("\nSuccessfully established handshake!\n");
}

void ClientBase::HandleMessage_AcceptConnect()
{
    myHasEstablishedConnection = true;
    printf("\nSuccessfully established connection!\n");
}