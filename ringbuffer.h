#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <stdint.h>

typedef struct
{

	uint64_t readPointer;
	uint64_t writePointer;
	int64_t size;

	void *buffer[];

} RingBuffer;

RingBuffer *ringBufferInit(int64_t size);
int ringBufferAdd(RingBuffer *buffer, void *value);
void * ringBufferGet(RingBuffer *buffer);
void ringBufferDestroy(RingBuffer *buffer);

#endif
