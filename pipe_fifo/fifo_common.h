#ifndef FIFO_COMMON_H
#define FIFO_COMMON_H

#include <stdio.h>
#include <time.h>

#define FIFO_PATH "/tmp/fifo"
#define READER_DELAY_SECONDS 10
#define MSG_SIZE 128

static inline void current_time_str(char *buf, size_t size)
{
    time_t now = time(NULL);
    struct tm tm_info;
    localtime_r(&now, &tm_info);
    strftime(buf, size, "%H:%M:%S", &tm_info);
}

#endif
