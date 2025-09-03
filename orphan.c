#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    pid_t pid = fork();

    if (pid > 0) {
        // Parent process
        printf("Parent (PID: %d) exiting...\n", getpid());
        exit(0);  // Parent exits immediately
    } 
    else if (pid == 0) {
        // Child process
        sleep(5);  // Give time for parent to exit
        printf("Child (PID: %d), new parent PID: %d\n", getpid(), getppid());
    } 
    else {
        perror("fork failed");
        exit(1);
    }

    return 0;
}
