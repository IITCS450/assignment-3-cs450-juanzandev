I was able to implement the scheduler and it has the following components:
1. per process tickets: as the PDF said, I included int tickets to struct proc and initialized it to 1 for all new processes
2. settickets(int n) Syscall: returns 0 if nothing goes wrong and -1 if it failed. It makes sure to validate n >= 1 and added a safety feature for access concurrency by acquiring ptable.lock, then it updates the calling process's ticket count
3. scheduler(): computes total tickets from all RUNNABLE proceses and draws a random winner between ticket number 0 and ticket max [0, total]. I used LCG PRNG
4. I used the basic test program testlottery.c and I also made lottery_measure.c which does the following: it makes sure that the scheduler is giving proportional CPU time to ticket counts. 3 child processes are created (each running in the same loop). Each child then records how many clock ticks it takes to finish and prints that result. Once all the children are done, then we can see if the child with more tickets finished sooner (what is expected) than the children with fewer tickets, since the probability of their tickets being picked out randomly is lower.

Testing: Ran a test with 3 child processes running on the same CPU bound loop. Each process has a different ratio of ticket allocation (1:2:4) and the testing metric is the cpu time per process or ticks. My hypothesis is for the process with x4 tickets would get + x4 cpu time meaning that it would finish 4  times faster than the other 2 child processes.

After running lottery_measure in QEMU I got the following results: 

| Iterations | Child0:Child1:Child2 | Ratio to Child2 |
|-----------|---------------------|-----------------|
| 500k      | 35:28:14            | 2.50:2.00:1.00  |
| 1M        | 81:68:51            | 1.59:1.33:1.00  |
| 1M        | 82:63:46            | 1.78:1.37:1.00  |
| 2M        | 162:131:99          | 1.64:1.32:1.00  |

In the first run with 500k tickets, the 4x child was 2.5x times faster than the third child even though I expected something closer to 4 times faster. The ratios seemed to improve with more work when I did runs with 1M and 2M tickets. What is certain is that Child2 always finishes first and Child0 always finishes last, and the ratios are closer to 1.7:1.3:1 rather than 4:2:1.

I think this variance in the scheduler fairness is because:
1. I performed runs with low ticket amounts meaning that randomness has higher relative impact.
2. Ticket distribution fluctuates and longer runs would run more realistic averages
3. The OS has a fixed overhead for content switches and interrupt handling, so the more tickets the lower the overhead impact is.

