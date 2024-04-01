#ifndef __EZLOPI_CORE_BUFFER_H__
#define __EZLOPI_CORE_BUFFER_H__

#include <string.h>
#include <stdint.h>

typedef enum e_buffer_state
{
    EZ_BUFFER_STATE_NOT_INITIATED = -1,
    EZ_BUFFER_STATE_AVAILABLE = 0,
    EZ_BUFFER_STATE_BUSY = 1,

} e_buffer_state_t;

e_buffer_state_t ezlopi_core_buffer_status(void);

void ezlopi_core_buffer_deinit(void);
void ezlopi_core_buffer_init(uint32_t len);

void ezlopi_core_buffer_release(void);
char *ezlopi_core_buffer_acquire(uint32_t *len, uint32_t wait_to_acquired_ms);

#endif // __EZLOPI_CORE_BUFFER_H__
