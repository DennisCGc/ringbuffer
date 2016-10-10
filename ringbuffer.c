/**
 * Implements a simple lock-free circular buffer that can be used as a bounded queue.
 *
 * This implementation is safe for multiple producers and consumers.
 *
 * Dennis Futselaar (C) 2016. Released under the 2-clause BSD license.
 */

#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "ringbuffer.h"

RingBuffer *ringBufferInit(int64_t size)
{
	RingBuffer *buffer = malloc(sizeof(RingBuffer) + size);

	buffer->readPointer = 0;
	buffer->writePointer = 0;
	buffer->size = size;
	memset(buffer->buffer, 1, size * sizeof(void*));
	return buffer;
}

int ringBufferAdd(RingBuffer *buffer, void *value)
{
	uint64_t readPointer;
	uint64_t writePointer;
	uint64_t realPointer;
	void *oldValue;

	while (1)
	{
		readPointer = __atomic_load_n(&buffer->readPointer, __ATOMIC_SEQ_CST);
		writePointer = __atomic_load_n(&buffer->writePointer, __ATOMIC_SEQ_CST);
		if (writePointer - readPointer >= buffer->size)
		{
			return 0;
		}

		realPointer = writePointer % buffer->size;
		oldValue = buffer->buffer[realPointer];
		if (!oldValue || !__atomic_compare_exchange_n(&buffer->buffer[realPointer], &oldValue, 0, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST))
		{
			continue;
		}

		if (__atomic_compare_exchange_n(&buffer->writePointer, &writePointer, writePointer + 1, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST))
		{
			break;
		}

		void *allocValue = NULL;
		__atomic_compare_exchange_n(&buffer->buffer[realPointer], &allocValue, oldValue, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
	}

	buffer->buffer[realPointer] = value;
	return 1;
}

void * ringBufferGet(RingBuffer *buffer)
{
	uint64_t readPointer;
	uint64_t writePointer;
	uint64_t realPointer;
	void *result;

	while (1)
	{
		writePointer = __atomic_load_n(&buffer->writePointer, __ATOMIC_SEQ_CST);
		readPointer = __atomic_load_n(&buffer->readPointer, __ATOMIC_SEQ_CST);
		if (readPointer >= writePointer)
		{
			return NULL;
		}

		realPointer = readPointer % buffer->size;
		result = buffer->buffer[realPointer];
		if (!result)
		{
			return NULL;
		}

		if (__atomic_compare_exchange_n(&buffer->readPointer, &readPointer, readPointer + 1, 0, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST))
		{
			break;
		}
	}

	return result;
}

void ringBufferDestroy(RingBuffer *buffer)
{
	free(buffer);
}
