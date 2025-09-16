#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

/*
 Simple program: two threads share one printer.
 Comments are written step-by-step (from main -> threads -> exit)
 and explain why each call is made and what the library does internally.

 Compile: gcc -pthread semaphore_printer.c -o semaphore_printer
 Run:     ./semaphore_printer
*/

/* Global semaphore representing the printer.
   We treat it as a binary semaphore by initializing to 1 (free).
   sem_t holds: an integer counter + kernel-managed wait-queue (handled by sem_* calls).
*/
sem_t printer;

/* Thread function: represents a user submitting a print job.
   Steps inside this function are numbered and explained.
*/
void* print_job(void* arg) {
    char* name = (char*) arg;

    /* STEP A: Try to acquire the printer.
       sem_wait is an atomic library/kernel call:
         - If semaphore value > 0: sem_wait atomically decrements it (value--) and returns immediately.
         - If semaphore value == 0: sem_wait blocks this thread and the kernel puts it on the semaphore's wait-queue.
       Important: you do NOT write "printer--" yourself — the decrement and blocking logic is done inside sem_wait.
       Reason: manual decrement would cause race conditions; sem_wait is implemented to be atomic and safe.
    */
    sem_wait(&printer);

    /* STEP B: Now the thread has exclusive access to the printer (critical section).
       Only one thread can reach this point at a time because semaphore value was decremented to 0.
       We simulate the printing work with sleep(2).
    */
    printf("%s is using the printer...\n", name);
    sleep(2); // simulate printing time (critical section)
    printf("%s finished printing.\n", name);

    /* STEP C: Release the printer.
       sem_post is also an atomic call:
         - It increments the semaphore value (value++).
         - If there are threads blocked on this semaphore, the kernel wakes up (unblocks) one of them.
       Reason: sem_post signals "resource available" and transfers control to one waiting thread if present.
       Note: because semaphores don't enforce ownership, any thread can call sem_post (unlike a mutex).
    */
    sem_post(&printer);

    return NULL;
}

int main() {
    pthread_t t1, t2;

    /* STEP 1: Initialize semaphore
       sem_init(&printer, pshared, value)
         - &printer : address of semaphore object
         - pshared = 0 : semaphore is shared between threads of the same process (not between processes)
         - value = 1 : initial count (1 means printer is free). Initializing to 1 makes this behave like a binary semaphore.
       Reason: we want exactly one user to use the printer at a time, so start with 1 available "slot".
    */
    sem_init(&printer, 0, 1);

    /* STEP 2: Create thread t1 (User1)
       pthread_create tells the OS to start a new thread that runs print_job("User1").
       Scheduling is up to the OS: thread may run immediately or later.
       Reason: we spawn two concurrent actors to demonstrate contention on the printer.
    */
    pthread_create(&t1, NULL, print_job, "User1");

    /* STEP 3: Create thread t2 (User2)
       Same as above. Now we have two threads that might race to acquire the semaphore.
       Reason: to show that only one thread can proceed while the other blocks inside sem_wait.
    */
    pthread_create(&t2, NULL, print_job, "User2");

    /* STEP 4: What happens now (runtime behavior — not explicit code)
       - Either t1 or t2 reaches sem_wait first (depends on scheduler).
       - First arriver sees value == 1 -> sem_wait decrements to 0 -> it proceeds into the critical section.
       - The other arrives and finds value == 0 -> sem_wait blocks that thread and it sleeps until sem_post.
       Reason: this ensures mutual exclusion on the printer resource.
    */

    /* STEP 5: Wait for threads to finish
       pthread_join(t1, NULL) blocks the main thread until t1 finishes.
       pthread_join(t2, NULL) blocks until t2 finishes.
       Reason: if main returned earlier, program might exit while threads are still running (bad).
       Also we must ensure semaphore is not destroyed while a thread could still use it.
    */
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    /* STEP 6: Cleanup
       sem_destroy releases any resources associated with the semaphore object.
       Reason: good practice to destroy what you initialize. Only do this after all threads are done.
    */
    sem_destroy(&printer);

    return 0;
}
