#include "NetBuffer.h"

void NetBuffer::ResetBuffer()
{
	myReadWriteIndex = 0;
	memset(myBuffer, 0, NetworkDefines::defaultBufferSize);
}