#pragma once
#include <corecrt_memcpy_s.h>
#include <cassert>
#include "NetworkDefines.hpp"

class NetBuffer
{
public:
	void ResetBuffer();
	const char* GetBuffer() const { return myBuffer; }
	char* GetBuffer() { return myBuffer; }
	int GetSize() const { return myReadWriteIndex; }

	template<typename T>
	void ReadData(T& aDataToWriteTo);

	template<typename T>
	void WriteData(const T& aDataToReadFrom);

	template<typename T>
	void WriteData(const T& aDataToReadFrom, int aSizeOfDataToReadFrom);

private:
	char myBuffer[NetworkDefines::defaultBufferSize]{ 0 };
	int myReadWriteIndex = 0;
};

template<typename T>
inline void NetBuffer::ReadData(T& aDataToWriteTo)
{
	assert(myReadWriteIndex < NetworkDefines::defaultBufferSize);

	constexpr int numBytes = static_cast<int>(sizeof(T));
	memcpy_s(&aDataToWriteTo, numBytes, myBuffer + myReadWriteIndex, numBytes);
	myReadWriteIndex += numBytes;
}

template<typename T>
inline void NetBuffer::WriteData(const T& aDataToReadFrom)
{
	assert(myReadWriteIndex < NetworkDefines::defaultBufferSize);

	constexpr int numBytes = static_cast<int>(sizeof(T));
	memcpy_s(myBuffer + myReadWriteIndex, NetworkDefines::defaultBufferSize - myReadWriteIndex, &aDataToReadFrom, numBytes);
	myReadWriteIndex += numBytes;
}

template<typename T>
inline void NetBuffer::WriteData(const T& aDataToReadFrom, int aSizeOfDataToReadFrom)
{
	if (aSizeOfDataToReadFrom > NetworkDefines::defaultBufferSize - myReadWriteIndex)
	{
		aSizeOfDataToReadFrom = NetworkDefines::defaultBufferSize - myReadWriteIndex;
	}

	memcpy_s(myBuffer + myReadWriteIndex, NetworkDefines::defaultBufferSize - myReadWriteIndex, &aDataToReadFrom, aSizeOfDataToReadFrom);
	myReadWriteIndex += aSizeOfDataToReadFrom;
}