#include "ring_buffer.h"

void RingBuffer_Init(RingBuffer *rb, uint8_t *buf, uint16_t size)
{
    rb->buffer = buf;
    rb->size = size;
    rb->head = 0;
    rb->tail = 0;
}

uint8_t RingBuffer_Put(RingBuffer *rb, uint8_t data)
{
    uint16_t next = (rb->head + 1) % rb->size;

    if (next == rb->tail) {
        // Buffer Full
        return 0;
    }

    rb->buffer[rb->head] = data;
    rb->head = next;
    return 1;
}

uint8_t RingBuffer_Get(RingBuffer *rb, uint8_t *data)
{
    if (rb->head == rb->tail) {
        // Buffer Empty
        return 0;
    }

    *data = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % rb->size;
    return 1;
}

uint8_t RingBuffer_IsEmpty(RingBuffer *rb)
{
    return (rb->head == rb->tail);
}

uint8_t RingBuffer_IsFull(RingBuffer *rb)
{
    return ((rb->head + 1) % rb->size == rb->tail);
}

uint16_t RingBuffer_Available(RingBuffer *rb)
{
    if (rb->head >= rb->tail) {
        return rb->head - rb->tail;
    } else {
        return rb->size - (rb->tail - rb->head);
    }
}
