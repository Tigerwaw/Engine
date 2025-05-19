#include "ServerBase.h"
#include <iostream>

#include "NetMessage.h"

ServerBase::ServerBase()
{
    myComm.Init(true, true, "");
    myReceiveThread = std::thread(&ServerBase::Receive, this);
}

ServerBase::~ServerBase()
{
    myReceiveThread.join();
    myComm.Destroy();
}

void ServerBase::Update()
{
}

void ServerBase::Receive()
{
    while (myShouldReceive)
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
                delete receivedMessage;
            }
        }
    }
}

void ServerBase::SendToAllClients(NetBuffer& aBuffer) const
{
    int clientIndex = 0;
    for (auto& client : myClients)
    {
        myComm.SendData(aBuffer, client.address);
        ++clientIndex;
        //printf("Sent data to client: %i\n", clientIndex);
    }
}

void ServerBase::SendToAllClientsExcluding(NetBuffer& aBuffer, const int aClientIndex) const
{
    for (auto& client : myClients)
    {
        if (GetClientIndex(client.address) == aClientIndex) continue;

        myComm.SendData(aBuffer, client.address);
    }
}

bool ServerBase::DoesClientExist(const sockaddr_in& aAddress) const
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

const int ServerBase::GetClientIndex(const sockaddr_in& aAddress) const
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