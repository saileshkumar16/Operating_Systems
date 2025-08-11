#include <stdio.h>
#include <unistd.h>

int main() 
{
    int id, a = 10;
    
    id = fork();
    
    if (id == 0)  // Child
    {
        a = a + 5;
        printf("Child PID: %d | &a: %p | a: %d\n", getpid(), (void*)&a, a);
    }
    else if (id > 0) // Parent
    {
        a = a - 5;
        printf("Parent PID: %d | &a: %p | a: %d\n", getpid(), (void*)&a, a);
    }

    return 0;
}
