#include <stdio.h>     // Standard I/O library for printf
#include <pthread.h>   // POSIX threads library for pthread_create, pthread_join

// Function that will be executed by the new thread
void* task_master(void* args)
{
    printf("Inside task master\n");   // This line runs inside the new thread
    return NULL;                      // Thread finishes execution, returns NULL
}

int main()
{
    pthread_t t_id;   // Declare a thread identifier (handle to refer to the new thread)
    
    printf("We are inside the main before creating thread\n");
    // At this point, only the main thread exists
    
    // Create a new thread
    // Arguments:
    //  &t_id      → store ID of the new thread
    //  NULL       → default thread attributes
    //  task_master→ function to execute in the new thread
    //  NULL       → arguments to pass to that function
    pthread_create(&t_id, NULL, task_master, NULL);
    
    // Wait for the newly created thread to finish
    // Without this, main could continue and exit before task_master finishes
    pthread_join(t_id, NULL);
    
    printf("We are inside the main After join\n");
    // This prints only after the thread has completed
    
    return 0;   // main ends, program terminates cleanly
}
