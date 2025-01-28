#pragma once
#include <vector>
#include "NetBuffer.h"

struct addrinfo;
struct sockaddr_in;

class Communicator
{
public:
	void Init(bool aIsBinding, bool aIsBlocking, const char* aIP);
	void Destroy();
	bool SendData(const NetBuffer& inData, const sockaddr_in& aRecipient) const;
	int ReceiveData(NetBuffer& outData, sockaddr_in& outSender) const;

	const sockaddr_in& GetAddress() const;
	const unsigned __int64& GetSocket() const { return mySocket; }

private:
	unsigned __int64 mySocket = NULL;
	addrinfo* myAddressInfo = NULL;
};