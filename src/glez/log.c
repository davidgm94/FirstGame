#include <glez/type.h>
#include <glez/log.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h>
#include <assert.h>
#define STB_SPRINTF_IMPLEMENTATION
#include <stb_sprintf.h>
#if _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

frametime_record past_frame;
frametime_record current_frame;
#define PRINT_STRING_BUFFER_CHAR_COUNT (MEGABYTES(5))
static char _g_print_string_buffer[PRINT_STRING_BUFFER_CHAR_COUNT];
static char* _g_print_string_buffer_ptr = _g_print_string_buffer;

#if _WIN32
static inline double compute_ms(s64 start, s64 end)
{
    return (double)((end - start) * 1000) / __game_performance_freq;
}
#else
static inline double compute_ms(struct timespec start, struct timespec end)
{
    const u64 start_ns = (u64)start.tv_sec * (u64)10e9 + (u64)start.tv_nsec;
    const u64 end_ns = (u64)end.tv_sec * (u64)10e9 + (u64)end.tv_nsec;
    const u64 ns = end_ns - start_ns;
    const double ns_to_ms_conversion_factor = 1000000;
    return ns / ns_to_ms_conversion_factor;
}
#endif

static inline size_t dont_format(const char* str)
{
    size_t len = strlen(str);
    for (u64 i = 0; i < len; i++)
    {
        char c = str[i];
        if (c == '%') {
            return 0;
        }
    }

    return len;
}

// TODO: look for multiple ways to log information(stdout, OutputDebugString, file...)
extern void frame_logger(const char* fmt, ...)
{
#if LOGS
    size_t len = dont_format(fmt);
    if (len) {
#define SHOULD_LOG_WRITTEN_BYTES_INFO 0
#if SHOULD_LOG_WRITTEN_BYTES_INFO
        printf("Bytes written to buffer: %zu\n", len);
#endif
        strcpy(_g_print_string_buffer_ptr, fmt);
        _g_print_string_buffer_ptr += len;
        return;
    }

    va_list args;
    va_start(args, fmt);
    int bytes_written = stbsp_vsprintf(_g_print_string_buffer_ptr, fmt, args);
    va_end(args);
    assert(_g_print_string_buffer_ptr + bytes_written < _g_print_string_buffer + PRINT_STRING_BUFFER_CHAR_COUNT);
    _g_print_string_buffer_ptr += (u64)bytes_written;
#endif
}

extern void frame_log_and_clear(void)
{
    for (u32 i = 0; i < TIME_FRAME_ELEMENT_COUNT; i++)
    {
        current_frame.record[i].ms = compute_ms(current_frame.record[i].start, current_frame.record[i].end);
    }

    current_frame.record[TIME_FRAME_CPU].ms = current_frame.record[TIME_FRAME_TOTAL].ms - current_frame.record[TIME_FRAME_GPU].ms;
    frame_logger("NEW FRAME:\n");
    for (u32 i = 0; i < TIME_FRAME_ELEMENT_COUNT; i++)
    {
        frame_logger("\t* [%s] %.02f ms.\n", TIME_BLOCK_STRING[i], current_frame.record[i].ms);
    }
    frame_logger("\t* [TIME_FRAME_SUMMARY] CPU: %.02f%%. GPU: %.02f%%\n",
                 (current_frame.record[TIME_FRAME_CPU].ms / current_frame.record[TIME_FRAME_TOTAL].ms) * 100.0f,
                 (current_frame.record[TIME_FRAME_GPU].ms / current_frame.record[TIME_FRAME_TOTAL].ms) * 100.0f);
    //struct timespec start, end, end2;
    //clock_gettime(CLOCK_MONOTONIC, &start);
#if _WIN32
    OutputDebugStringA(_g_print_string_buffer);
#else
    fwrite(_g_print_string_buffer, _g_print_string_buffer_ptr - _g_print_string_buffer, 1, stdout);
#endif
    //clock_gettime(CLOCK_MONOTONIC, &end);
    //write(1, _g_print_string_buffer, _g_print_string_buffer_ptr - _g_print_string_buffer);
    //clock_gettime(CLOCK_MONOTONIC, &end2);
    //f64 crt = compute_ms(start, end);
    //f64 syscall = compute_ms(end, end2);
    //printf("%f crt, %f syscall", crt, syscall);
    // Reset the buffer
    _g_print_string_buffer_ptr = _g_print_string_buffer;
}
