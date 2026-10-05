#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>

#define CHILD_EXIT_CODE 192

static void atexit_handler(void)
{
    printf("[atexit] процесс PID=%d завершается, вызван обработчик atexit()\n",
           getpid());
    fflush(stdout);
}

static void wwrite(const char *buf, size_t len)
{
    ssize_t r = write(1, buf, len);
    (void)r;
}

static void sigint_handler(int signo)
{
    (void)signo;
    const char msg[] =
        "\n[SIGINT] получен сигнал 2 (SIGINT): прерывание с клавиатуры (Ctrl+C)\n";
    wwrite(msg, sizeof(msg) - 1);
}

static void sigterm_handler(int signo, siginfo_t *info, void *context)
{
    (void)signo;
    (void)context;
    const char msg[] =
        "\n[SIGTERM] получен сигнал 15 (SIGTERM): запрос на завершение процесса\n";
    wwrite(msg, sizeof(msg) - 1);
    char buf[64];
    int n = snprintf(buf, sizeof(buf), "[SIGTERM] отправитель: PID=%d\n", (int)info->si_pid);
    if (n > 0) {
        wwrite(buf, (size_t)n);
    }
}

static void setup_signals(void)
{
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("signal(SIGINT)");
        exit(EXIT_FAILURE);
    }
 
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_sigaction = sigterm_handler;
    sa.sa_flags = SA_SIGINFO;  
    sigemptyset(&sa.sa_mask);
 
    if (sigaction(SIGTERM, &sa, NULL) == -1) {
        perror("sigaction(SIGTERM)");
        exit(EXIT_FAILURE);
    }
}

int main(void)
{
    if (atexit(atexit_handler) != 0) {
	fprintf(stderr, "не удалось зарегистрировать atexit-обработчик\n");
    	return EXIT_FAILURE;
    }
    setup_signals();
    printf("[start] исходный процесс: PID=%d, PPID=%d\n", getpid(), getppid());
    fflush(stdout);
 
    pid_t pid = fork();
 
    if (pid == -1) {
        perror("fork");
        return EXIT_FAILURE;
    }
 
    if (pid == 0) {
        printf("[child]  PID=%d, PPID=%d\n", getpid(), getppid());
        fflush(stdout);
 
        printf("[child]  работаю 5 секунд (можно послать сигнал)...\n");
        fflush(stdout);
        sleep(5);
 
        printf("[child]  завершаюсь с кодом %d\n", CHILD_EXIT_CODE);
        fflush(stdout);
        exit(CHILD_EXIT_CODE);
    }
 
    printf("[parent] PID=%d, PPID=%d, PID потомка=%d\n",
           getpid(), getppid(), pid);
    fflush(stdout);
 
    int status;
    pid_t waited;
 
    do {
        waited = waitpid(pid, &status, 0);
    } while (waited == -1 && errno == EINTR);
 
    if (waited == -1) {
        perror("waitpid");
        return EXIT_FAILURE;
    }
 
    if (WIFEXITED(status)) {
        printf("[parent] потомок %d завершился нормально, код завершения = %d\n",
               (int)waited, WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
        printf("[parent] потомок %d убит сигналом %d (%s)\n",
               (int)waited, WTERMSIG(status), strsignal(WTERMSIG(status)));
    } else {
        printf("[parent] потомок %d завершился неизвестным образом\n", (int)waited);
    }
 
    return EXIT_SUCCESS;
}
 
