#include "Communicator.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdio.h>

#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")

#define DEFAULT_PORT "27015"

void Communicator::Init(bool aIsBinding, bool aIsBlocking, const char* aIP)
{
    // Initialize Winsock
    WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        printf("WSAStartup failed: %d\n", result);
        return;
    }

    addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = IPPROTO_UDP;

    if (aIsBinding)
    {
        hints.ai_flags = AI_PASSIVE;
    }

    result = getaddrinfo(NULL, DEFAULT_PORT, &hints, &myAddressInfo);
    if (result != 0) {
        printf("getaddrinfo failed with error: %d\n", result);
        WSACleanup();
        return;
    }

    if (!aIsBinding && strlen(aIP) > 0)
    {
        sockaddr_in* addr = reinterpret_cast<sockaddr_in*>(myAddressInfo->ai_addr);
        result = inet_pton(AF_INET, aIP, &addr->sin_addr.S_un.S_addr);
        if (result == SOCKET_ERROR)
        {
            printf("failed to get address with error: %d\n", WSAGetLastError());
            freeaddrinfo(myAddressInfo);
            closesocket(mySocket);
            WSACleanup();
            return;
        }
    }

    mySocket = socket(myAddressInfo->ai_family, myAddressInfo->ai_socktype, myAddressInfo->ai_protocol);
    if (mySocket == INVALID_SOCKET) {
        printf("Error at socket: %ld\n", WSAGetLastError());
        freeaddrinfo(myAddressInfo);
        WSACleanup();
        return;
    }

    if (!aIsBlocking)
    {
        unsigned long nonBlocking = 1;
        ioctlsocket(mySocket, FIONBIO, &nonBlocking);
    }

    if (aIsBinding)
    {
        result = bind(mySocket, myAddressInfo->ai_addr, static_cast<int>(myAddressInfo->ai_addrlen));
        if (result == SOCKET_ERROR) {
            printf("bind failed with error: %d\n", WSAGetLastError());
            freeaddrinfo(myAddressInfo);
            closesocket(mySocket);
            WSACleanup();
            return;
        }
    }
}

void Communicator::Destroy()
{
    closesocket(mySocket);
    freeaddrinfo(myAddressInfo);
    mySocket = NULL;
    WSACleanup();
}

bool Communicator::SendData(const NetBuffer& inData, const sockaddr_in& aRecipient) const
{
    int sendResult = sendto(mySocket, inData.GetBuffer(), inData.GetSize(), 0, reinterpret_cast<const sockaddr*>(&aRecipient), sizeof(sockaddr_in));
    if (sendResult == SOCKET_ERROR) {
        printf("send failed: %d\n", WSAGetLastError());
        closesocket(mySocket);
        WSACleanup();
        return false;
    }

    return true;
}

int Communicator::ReceiveData(NetBuffer& outData, sockaddr_in& outSender) const
{
    char buff[DEFAULT_BUFLEN]{ 0 };

    sockaddr_in recAddress = {};
    int recAddressLen = sizeof(sockaddr_in);
    int result = recvfrom(mySocket, buff, DEFAULT_BUFLEN, 0, reinterpret_cast<sockaddr*>(&recAddress), &recAddressLen);
    if (result > 0)
    {
        memcpy_s(outData.GetBuffer(), result, buff, result);
        outSender = recAddress;
        return result;
    }
    else if (result == 0)
    {
        printf("Connection closing...\n");
    }
    else
    {
        int errorMsg = WSAGetLastError();
        if (errorMsg != 10035 && errorMsg != 10054)
        {
            printf("recv failed: %d\n", WSAGetLastError());
            closesocket(mySocket);
            WSACleanup();
        }
    }

	return result;
}

const sockaddr_in& Communicator::GetAddress() const
{
    return reinterpret_cast<const sockaddr_in&>(*myAddressInfo->ai_addr);
}