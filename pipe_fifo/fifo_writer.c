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
    printf("[writer] PID=%d, открываю %s на запись\n",
           getpid(), FIFO_PATH);
    fflush(stdout);

    int fd = open(FIFO_PATH, O_WRONLY);
    if (fd == -1) {
        perror("open");
        return EXIT_FAILURE;
    }

    char now[16];
    current_time_str(now, sizeof(now));
    char msg[MSG_SIZE];
    int len = snprintf(msg, sizeof(msg), "Время писателя: %s, PID писателя: %d",
                       now, getpid());

    printf("[writer] время %s, отправляю: \"%s\"\n", now, msg);
    fflush(stdout);
    if (write(fd, msg, (size_t)len) == -1) {
        perror("write");
        close(fd);
        return EXIT_FAILURE;
    }
    close(fd);
    return EXIT_SUCCESS;
}
