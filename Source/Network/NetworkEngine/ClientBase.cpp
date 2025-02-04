#include "ClientBase.h"

#include "NetMessage.h"
#include <WinSock2.h>

ClientBase::ClientBase()
{
    myComm.Init(false, false, "10.250.224.81");
    myLastHandshakeRequestTime = std::chrono::system_clock::now();
}

ClientBase::~ClientBase()
{
    myReceiveThread.join();
    myComm.Destroy();
}

void ClientBase::StartReceive(ClientBase* aClient)
{
    myReceiveThread = std::thread(&ClientBase::Receive, aClient);
    SendHandshakeRequest();
}

void ClientBase::Receive()
{
    while (myShouldReceive)
    {
        if (!myHasEstablishedHandshake)
        {
            std::chrono::duration<float> elapsed_seconds = myLastHandshakeRequestTime - std::chrono::system_clock::now();
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
            NetMessage* receivedMessage = ReceiveMessage(receiveBuffer);

            if (receivedMessage)
            {
                receivedMessage->Deserialize(receiveBuffer);
                HandleMessage(receivedMessage);
                delete(receivedMessage);
            }
        }
    }
}

void ClientBase::HandleMessage_HandshakeAccept()
{
    myHasEstablishedHandshake = true;
}