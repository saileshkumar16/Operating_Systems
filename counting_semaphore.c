#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

/*
 Example: 6 students share 3 library tables.
 Counting semaphore initialized with 3 (for 3 tables).
 We will trace how the semaphore value changes with each sem_wait / sem_post.

 Compile: gcc -pthread semaphore_tables.c -o semaphore_tables
 Run:     ./semaphore_tables
*/

#define NUM_TABLES 3
#define NUM_STUDENTS 6

sem_t tables;  // counting semaphore

void* student(void* arg) 
{
    int id = *(int*)arg;

    /* STEP A: Try to acquire a table
       sem_wait:
         - If tables > 0 → decrement tables-- and continue
         - If tables == 0 → block until someone calls sem_post
       Example value changes:
         Start = 3
         First student arrives → sem_wait → 3 → 2
         Second student → sem_wait → 2 → 1
         Third student → sem_wait → 1 → 0
         Fourth student → finds 0 → BLOCKED
    */
    sem_wait(&tables);
    printf("Student %d got a table (semaphore decremented).\n", id);

    sleep(2); // simulate study time

    /* STEP B: Release the table
       sem_post:
         - Increment tables++
         - If any students are waiting, wake one of them
       Example value changes:
         When first student leaves → 0 → 1
         Wakes up Student 4 → immediately sem_wait inside that student → 1 → 0
    */
    printf("Student %d left the table (semaphore incremented).\n", id);
    sem_post(&tables);

    return NULL;
}

int main() {
    pthread_t tid[NUM_STUDENTS];
    int ids[NUM_STUDENTS];

    /* STEP 1: Initialize semaphore
       sem_init(&tables, 0, 3)
       Current value = 3 (3 tables free)
    */
    sem_init(&tables, 0, NUM_TABLES);

    /* STEP 2: Create 6 student threads
       Each will try sem_wait:
         - First 3 succeed immediately (3→2→1→0)
         - Remaining students block until tables free
    */
    for (int i = 0; i < NUM_STUDENTS; i++) 
    {
        ids[i] = i + 1;
        pthread_create(&tid[i], NULL, student, &ids[i]);
    }

    /* STEP 3: Join all threads
       Main thread waits until all students finish.
       Semaphore value will return to 3 at the end:
         - Every sem_wait (--) is matched by a sem_post (++).
    */
    for (int i = 0; i < NUM_STUDENTS; i++)
    {
        pthread_join(tid[i], NULL);
    }

    /* STEP 4: Destroy semaphore (cleanup) */
    sem_destroy(&tables);

    return 0;
}
