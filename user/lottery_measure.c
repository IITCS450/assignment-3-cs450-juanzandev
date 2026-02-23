#include "types.h"
#include "stat.h"
#include "user.h"

// CPU-bound work function
static volatile int sink = 0;
static void burn_cpu(int iterations)
{
    for (int i = 0; i < iterations; i++)
    {
        sink += i;
    }
}

int main(int argc, char **argv)
{
    // Test with 3 children: tickets 1, 2, 4
    // Expected CPU time ratio: 1:2:4 (inverse proportional to work when work is equal)

    int num_children = 3;
    int tickets[] = {1, 2, 4};
    int work_units = 500000; // Same work per child

    if (argc >= 2)
        work_units = atoi(argv[1]);

    printf(1, "=== Lottery CPU Share Test ===\n");
    printf(1, "Spawning %d children with tickets: 1, 2, 4\n", num_children);
    printf(1, "Each child does %d work units\n", work_units);


    for (int i = 0; i < num_children; i++)
    {
        int pid = fork();
        if (pid < 0)
        {
            printf(1, "fork failed\n");
            exit();
        }

        if (pid == 0)
        {
            // Child process
            int t = tickets[i];
            int start = uptime();

            if (settickets(t) != 0)
            {
                printf(1, "Child %d: settickets(%d) FAILED\n", i, t);
                exit();
            }

            // Do CPU work
            burn_cpu(work_units);

            int end = uptime();
            int elapsed = end - start;
            printf(1, "Child %d (tickets=%d): completed in %d ticks\n", i, t, elapsed);
            exit();
        }
    }

    // Parent waits for all children
    for (int i = 0; i < num_children; i++)
    {
        wait();
    }

    printf(1, "\n=== Test Complete ===\n");
    exit();
}
