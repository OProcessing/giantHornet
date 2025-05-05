#ifndef __RING_BUFFER_H__
#define __RING_BUFFER_H__

#include "stdint.h"

typedef struct {
    uint8_t *buffer;
    uint16_t size;  
    uint16_t head;  
    uint16_t tail;  
} RingBuffer;

void RingBuffer_Init(RingBuffer *rb, uint8_t *buf, uint16_t size);
uint8_t RingBuffer_Put(RingBuffer *rb, uint8_t data);
uint8_t RingBuffer_Get(RingBuffer *rb, uint8_t *data);
uint8_t RingBuffer_IsEmpty(RingBuffer *rb);
uint8_t RingBuffer_IsFull(RingBuffer *rb);
uint16_t RingBuffer_Available(RingBuffer *rb);

#endif /* __RING_BUFFER_H__ */
