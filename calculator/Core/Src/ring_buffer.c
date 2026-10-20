#include "ring_buffer.h"
#include "main.h"

void RingBuffer_Init(RingBuffer *rb) {
    RingBuffer_Clear(rb);
}

void RingBuffer_Clear(RingBuffer *rb) {
    uint32_t primask = __get_PRIMASK();
    __disable_irq();

    rb->head = 0;
    rb->tail = 0;
    rb->count = 0;

    __set_PRIMASK(primask);
}

bool RingBuffer_Push(RingBuffer *rb, uint8_t byte) {
    uint32_t primask = __get_PRIMASK();
    __disable_irq();

    if (rb->count >= RING_BUFFER_SIZE) {
        __set_PRIMASK(primask);
        return false;
    }

    rb->buffer[rb->head] = byte;
    rb->head = (rb->head + 1) % RING_BUFFER_SIZE;
    rb->count++;

    __set_PRIMASK(primask);
    return true;
}

bool RingBuffer_Pop(RingBuffer *rb, uint8_t *byte) {
    uint32_t primask = __get_PRIMASK();
    __disable_irq();

    if (rb->count == 0) {
        __set_PRIMASK(primask);
        return false;
    }

    *byte = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % RING_BUFFER_SIZE;
    rb->count--;

    __set_PRIMASK(primask);
    return true;
}

bool RingBuffer_IsEmpty(const RingBuffer *rb) {
    return (rb->count == 0);
}
