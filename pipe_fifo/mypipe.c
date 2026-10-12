#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/wait.h>

#define CHILD_DELAY_SECONDS 5
#define MSG_SIZE 128

static void current_time_str(char *buf, size_t size)
{
    time_t now = time(NULL);
    struct tm tm_info;
    localtime_r(&now, &tm_info);
    strftime(buf, size, "%H:%M:%S", &tm_info);
}

int main(void)
{
    int fd[2];

    if (pipe(fd) == -1) {
        perror("pipe");
        return EXIT_FAILURE;
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        /*читатель*/
        close(fd[1]);

        printf("[child]  PID=%d, сплю %d сек перед чтением из pipe...\n",
               getpid(), CHILD_DELAY_SECONDS);
        fflush(stdout);
        sleep(CHILD_DELAY_SECONDS);

        char msg[MSG_SIZE];
        size_t total = 0;
        ssize_t n;

        while ((n = read(fd[0], msg + total, sizeof(msg) - 1 - total)) > 0) {
            total += (size_t)n;
        }
        if (n == -1) {
            perror("read");
            exit(EXIT_FAILURE);
        }
        msg[total] = '\0'; /* read() не добавляет '\0' - ставим сами */
        close(fd[0]);

        char now[16];
        current_time_str(now, sizeof(now));
        printf("[child]  текущее время: %s\n", now);
        printf("[child]  получено через pipe: \"%s\"\n", msg);
        fflush(stdout);
        exit(EXIT_SUCCESS);
    }

    /*писатель*/

    close(fd[0]);

    char now[16];
    current_time_str(now, sizeof(now));

    char msg[MSG_SIZE];
    int len = snprintf(msg, sizeof(msg), "Время родителя: %s, PID родителя: %d",
                       now, getpid());

    printf("[parent] PID=%d, время %s, отправляю: \"%s\"\n", getpid(), now, msg);
    fflush(stdout);

    if (write(fd[1], msg, (size_t)len) == -1) {
        perror("write");
        return EXIT_FAILURE;
    }
    close(fd[1]);

    int status;
    if (waitpid(pid, &status, 0) == -1) {
        perror("waitpid");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
