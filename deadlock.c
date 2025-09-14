#include <stdio.h>      // for printf() -> to show which thread got which lock
#include <pthread.h>    // for pthreads (thread functions, mutexes)
#include <unistd.h>     // for sleep() -> simulate work/delay

// ---------------- GLOBAL SHARED RESOURCES ----------------
pthread_mutex_t lock1, lock2;   // two mutexes = two shared resources

// ---------------- THREAD FUNCTION: PROCESS 1 ----------------
void* process1(void* arg) {
    pthread_mutex_lock(&lock1);     // Process 1 acquires lock1 first
    printf("Process 1 acquired lock1\n");
    sleep(1);                       // delay so Process 2 can run and grab lock2
    
    pthread_mutex_lock(&lock2);     // Process 1 now waits for lock2
                                    // but if Process 2 already has lock2 → deadlock
    printf("Process 1 acquired lock2\n");

    // if both locks acquired → do critical section work
    pthread_mutex_unlock(&lock2);   // release lock2 (cleanup step)
    pthread_mutex_unlock(&lock1);   // release lock1
    return NULL;                    // thread finishes
}

// ---------------- THREAD FUNCTION: PROCESS 2 ----------------
void* process2(void* arg) {
    pthread_mutex_lock(&lock2);     // Process 2 acquires lock2 first
    printf("Process 2 acquired lock2\n");
    sleep(1);                       // delay so Process 1 can run and grab lock1
    
    pthread_mutex_lock(&lock1);     // Process 2 now waits for lock1
                                    // but if Process 1 already has lock1 → deadlock
    printf("Process 2 acquired lock1\n");

    // if both locks acquired → do critical section work
    pthread_mutex_unlock(&lock1);   // release lock1
    pthread_mutex_unlock(&lock2);   // release lock2
    return NULL;                    // thread finishes
}

// ---------------- MAIN FUNCTION ----------------
int main() {
    pthread_t t1, t2;               // thread identifiers (used to manage threads)

    // 1. Initialize mutexes before using them
    pthread_mutex_init(&lock1, NULL);   // create lock1
    pthread_mutex_init(&lock2, NULL);   // create lock2

    // 2. Create two threads → run process1 and process2 "in parallel"
    pthread_create(&t1, NULL, process1, NULL);  
    pthread_create(&t2, NULL, process2, NULL);

    // 3. Wait for both threads to finish
    pthread_join(t1, NULL);         // main thread waits until t1 finishes
    pthread_join(t2, NULL);         // main thread waits until t2 finishes
                                    // prevents program from exiting early

    // 4. Destroy mutexes after use (good practice, prevents resource leaks)
    pthread_mutex_destroy(&lock1);
    pthread_mutex_destroy(&lock2);

    return 0;                       // program ends cleanly
}
