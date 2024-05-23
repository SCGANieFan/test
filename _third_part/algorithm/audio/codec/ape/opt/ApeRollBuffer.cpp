#include"ApePrive.h"
#include <string.h>


bool ApeRollBufferInit(ApeRollBuffer* rollBuffer, uint8_t* buffer, uint32_t bufferLen, uint32_t bufferLenRemMin)
{
	rollBuffer->buffer = buffer;
	rollBuffer->bufferLen = bufferLen;
	rollBuffer->bufferLenRemMin = bufferLenRemMin;
	rollBuffer->offset = 0;
	rollBuffer->bufferLenRem = bufferLen;
	return true;
}

void ApeRollBufferIncrement(ApeRollBuffer* rollBuffer, uint32_t bytes)
{
#if 1
	rollBuffer->offset += bytes;
	rollBuffer->bufferLenRem -= bytes;
	if (rollBuffer->bufferLenRem < rollBuffer->bufferLenRemMin)
	{
		// for (uint32_t i = 0; i < rollBuffer->bufferLenRem; i++)
		// {
		// 	*(rollBuffer->buffer + i) = *(rollBuffer->buffer + i + rollBuffer->offset);
		// }

		memcpy(rollBuffer->buffer, rollBuffer->buffer + rollBuffer->offset, rollBuffer->bufferLenRem);
		rollBuffer->offset = 0;
		rollBuffer->bufferLenRem = rollBuffer->bufferLen;
	}
#endif
}


