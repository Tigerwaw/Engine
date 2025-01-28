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
	void ReadData(T& aDataToWriteTo) const;

	template<typename T>
	void WriteData(const T& aDataToReadFrom);

	template<typename T>
	void WriteData(const T& aDataToReadFrom, int aSizeOfDataToReadFrom);

private:
	char myBuffer[DEFAULT_BUFLEN]{ 0 };
	int myReadWriteIndex = 0;
};

template<typename T>
inline void NetBuffer::ReadData(T& aDataToWriteTo) const
{
	memcpy_s(&aDataToWriteTo, DEFAULT_BUFLEN - 4, myBuffer + 4, DEFAULT_BUFLEN - 4);
}

template<typename T>
inline void NetBuffer::WriteData(const T& aDataToReadFrom)
{
	static constexpr size_t numBytes = sizeof(T);
	memcpy_s(myBuffer + myReadWriteIndex, DEFAULT_BUFLEN - myReadWriteIndex, &aDataToReadFrom, numBytes);
	myReadWriteIndex += static_cast<int>(numBytes);
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