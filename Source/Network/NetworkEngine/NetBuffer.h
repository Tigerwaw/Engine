#pragma once
#include <corecrt_memcpy_s.h>

#define DEFAULT_BUFLEN 512

class NetBuffer
{
public:
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
	char myBuffer[DEFAULT_BUFLEN]{ 0 };
	int myReadWriteIndex = 0;
};

template<typename T>
inline void NetBuffer::ReadData(T& aDataToWriteTo)
{
	constexpr int numBytes = static_cast<int>(sizeof(T));
	memcpy_s(&aDataToWriteTo, numBytes, myBuffer + myReadWriteIndex, numBytes);
	//memcpy_s(&aDataToWriteTo, DEFAULT_BUFLEN, myBuffer + 4, DEFAULT_BUFLEN - 4);
	myReadWriteIndex += numBytes;
}

template<typename T>
inline void NetBuffer::WriteData(const T& aDataToReadFrom)
{
	constexpr int numBytes = static_cast<int>(sizeof(T));
	memcpy_s(myBuffer + myReadWriteIndex, DEFAULT_BUFLEN - myReadWriteIndex, &aDataToReadFrom, numBytes);
	myReadWriteIndex += numBytes;
}

template<typename T>
inline void NetBuffer::WriteData(const T& aDataToReadFrom, int aSizeOfDataToReadFrom)
{
	if (aSizeOfDataToReadFrom > DEFAULT_BUFLEN - myReadWriteIndex)
	{
		aSizeOfDataToReadFrom = DEFAULT_BUFLEN - myReadWriteIndex;
	}

	memcpy_s(myBuffer + myReadWriteIndex, DEFAULT_BUFLEN - myReadWriteIndex, &aDataToReadFrom, aSizeOfDataToReadFrom);
	myReadWriteIndex += aSizeOfDataToReadFrom;
}