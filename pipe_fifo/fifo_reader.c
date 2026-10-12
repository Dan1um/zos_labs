#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include "fifo_common.h"

int main(void)
{
    if (mkfifo(FIFO_PATH, 0666) == -1 && errno != EEXIST) {
        perror("mkfifo");
        return EXIT_FAILURE;
    }
    printf("[reader] PID=%d, открываю %s на чтение (жду писателя)...\n",
           getpid(), FIFO_PATH);
    fflush(stdout);

    int fd = open(FIFO_PATH, O_RDONLY);
    if (fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }
    printf("[reader] FIFO открыт, сплю %d сек перед чтением...\n", READER_DELAY_SECONDS);
    fflush(stdout);
    sleep(READER_DELAY_SECONDS);

    char msg[MSG_SIZE];
    size_t total = 0;
    ssize_t n;
    while ((n = read(fd, msg + total, sizeof(msg) - 1 - total)) > 0) {
        total += (size_t)n;
    }
    if (n == -1) {
        perror("read");
        close(fd);
        return EXIT_FAILURE;
    }
    msg[total] = '\0'; /* read() не добавляет '\0' - ставим сами */
    close(fd);

    char now[16];
    current_time_str(now, sizeof(now));
    printf("[reader] текущее время: %s\n", now);
    printf("[reader] получено через FIFO: \"%s\"\n", msg);
    unlink(FIFO_PATH);
    return EXIT_SUCCESS;
}
