#include "ServerBase.h"
#include <iostream>
#include <cassert>

#include "NetMessage.h"

ServerBase::~ServerBase()
{
    myComm.Destroy();
}

void ServerBase::StartServer()
{
    myComm.Init(true, false, "");
    myShouldReceive = true;
}

void ServerBase::Update()
{
    std::chrono::duration<float> elapsed_seconds = std::chrono::system_clock::now() - myLastDataTickTime;
    if (elapsed_seconds.count() > myDataTickRate)
    {
        myLastDataTickTime = std::chrono::system_clock::now();
        myAvgDataReceived = static_cast<int>(std::roundf(myDataReceived / elapsed_seconds.count()));
        myAvgDataSent = static_cast<int>(std::roundf(myDataSent / elapsed_seconds.count()));
        myDataReceived = 0;
        myDataSent = 0;
    }

    if (myShouldReceive)
    {
        Receive();
    }
}

void ServerBase::Receive()
{
    for (int i = 0; i < myMessagesHandledPerTick; i++)
    {
        sockaddr_in otherAddress = {};
        NetBuffer receiveBuffer;
        int bytesReceived = myComm.ReceiveData(receiveBuffer, otherAddress);
        if (bytesReceived > 0)
        {
            NetMessage* receivedMessage = ReceiveMessage(receiveBuffer);

            if (receivedMessage)
            {
                receivedMessage->Deserialize(receiveBuffer);
                HandleMessage(receivedMessage, otherAddress, bytesReceived);
                delete receivedMessage;
            }
        }
        else
        {
            break;
        }
    }
}

void ServerBase::AcceptHandshake(const NetBuffer& aBuffer, const sockaddr_in& aAddress)
{
    myComm.SendData(aBuffer, aAddress);
    printf("\nAccepted handshake for adress [%i] : [%i]", aAddress.sin_addr.S_un.S_addr, aAddress.sin_port);
}

const NetInfo& ServerBase::AddClient(const sockaddr_in& aAddress, const std::string& aUsername)
{
    NetInfo& newClient = myClients.emplace_back();
    newClient.address = aAddress;
    newClient.username = aUsername;

    printf("Added client %s : %i", aUsername.c_str(), aAddress.sin_addr.S_un.S_addr);
    return newClient;
}

void ServerBase::RemoveClient(int aClientIndex)
{
    myClients.erase(myClients.begin() + aClientIndex);
}

void ServerBase::SendToClient(const NetBuffer& aBuffer, int aClientIndex)
{
    assert(aClientIndex >= 0 && aClientIndex < static_cast<int>(myClients.size()));

    myDataSent += myComm.SendData(aBuffer, myClients[aClientIndex].address);
}

void ServerBase::SendToAllClients(const NetBuffer& aBuffer)
{
    for (int clientIndex = 0; clientIndex < static_cast<int>(myClients.size()); ++clientIndex)
    {
        SendToClient(aBuffer, clientIndex);
    }
}

void ServerBase::SendToAllClientsExcluding(const NetBuffer& aBuffer, const int aClientIndex)
{
    for (int clientIndex = 0; clientIndex < static_cast<int>(myClients.size()); ++clientIndex)
    {
        if (clientIndex == aClientIndex) continue;

        SendToClient(aBuffer, clientIndex);
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

const NetInfo& ServerBase::GetClient(int aClientIndex) const
{
    assert(aClientIndex >= 0 && aClientIndex < static_cast<int>(myClients.size()));

    return myClients[aClientIndex];
}