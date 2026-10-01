/**
 * @file port_host_timer.c
 * @brief Host PC System Timer Implementation.
 * 
 * @author S. Sherifred Singh (Senior Embedded Firmware Engineer)
 * @standard MISRA-C:2012 compliant, ANSI C99
 */

#include "../../hal/hal_timer.h"
#include <time.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#include <unistd.h>
#include <sys/time.h>
#endif

static bool s_manual_mode = false;
static uint32_t s_manual_ms = 0;

void hal_timer_init(void)
{
    s_manual_mode = false;
    s_manual_ms = 0;
}

uint32_t hal_timer_get_ms(void)
{
    if (s_manual_mode) {
        return s_manual_ms;
    }

#if defined(_WIN32) || defined(_WIN64)
    return (uint32_t)GetTickCount();
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint32_t)((tv.tv_sec * 1000) + (tv.tv_usec / 1000));
#endif
}

uint64_t hal_timer_get_us(void)
{
    if (s_manual_mode) {
        return (uint64_t)s_manual_ms * 1000ULL;
    }

#if defined(_WIN32) || defined(_WIN64)
    LARGE_INTEGER freq, counter;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&counter);
    return (uint64_t)((counter.QuadPart * 1000000ULL) / freq.QuadPart);
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return (uint64_t)((tv.tv_sec * 1000000ULL) + tv.tv_usec);
#endif
}

void hal_timer_delay_ms(uint32_t ms)
{
#if defined(_WIN32) || defined(_WIN64)
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

/* Host Test Helpers */
void port_host_timer_set_manual(bool enable)
{
    s_manual_mode = enable;
}

void port_host_timer_advance_ms(uint32_t ms)
{
    s_manual_ms += ms;
}
