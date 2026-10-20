#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdint.h>
#include <stdbool.h>

#define RING_BUFFER_SIZE 128

typedef struct {
    uint8_t buffer[RING_BUFFER_SIZE];
    volatile uint16_t head;
    volatile uint16_t tail;
    volatile uint16_t count;
} RingBuffer;

void RingBuffer_Init(RingBuffer *rb);
void RingBuffer_Clear(RingBuffer *rb);
bool RingBuffer_Push(RingBuffer *rb, uint8_t byte);
bool RingBuffer_Pop(RingBuffer *rb, uint8_t *byte);
bool RingBuffer_IsEmpty(const RingBuffer *rb);

#endif /* RING_BUFFER_H */
