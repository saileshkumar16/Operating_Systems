#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    pid_t pid = fork();

    if (pid > 0) {
        // Parent process
        printf("Parent (PID: %d) sleeping...\n", getpid());
        sleep(10); // During this time, child becomes zombie
        wait(NULL); // Parent collects child exit status
        printf("Parent collected child. Zombie cleared.\n");
    } 
    else if (pid == 0) {
        // Child process
        printf("Child (PID: %d) exiting now...\n", getpid());
        exit(0); // Child exits → becomes zombie until parent waits
    } 
    else {
        perror("fork failed");
        exit(1);
    }

    return 0;
}
