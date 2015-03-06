-----------------------------------------------------------Readme-------------------------------------------------
-----------------------------------------------------------------------------------------------------------------------
Author: Arunprasanna Sundararajan Poorna

Platform Used: 
Lubuntu(lightweight GUI Ubuntu)- 32bit running on VirtualBox. Host Os: Mac OS X Mavericks on Intel X86_64
gcc (Ubuntu/Linaro 4.6.3-1ubuntu5) 4.6.3
Copyright (C) 2011 Free Software Foundation, Inc.
This is free software; see the source for copying conditions.  There is NO
warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.


Implementations of Round Robin Scheduler:

Three essential implementation detail of the scheduler;
1. A list of current contexts is maintained and traversed every-time a thread is preempted, the next active thread is given time slice.
2. SIGVTALRM raises a signal after a certain time has elapsed, causing preemption.
3.Timer function is initialized and a virtual time is set for each thread to run.
4. Every thread starts and stops with the signal handler.
5. Makecontext, Swapontext are used.

To Compile all files and generate an executable binary for dining philosophers:
Type:

make

To run the dining philosophers type:

./diner

To run other programs, make and then:

1.	gcc -Wall -pedantic -I{..} -o program program.c gathered.a

2.	./program

Ensure program.c is in the same directory. 


Prevention of Deadlock in Dining Philosophers

->Prevention of deadlock is done using the lefty-righty solution.
 ->Deadlock occurs when all the philosophers are left or right handed and they contend for the same resource in a similar manner. 
->By making one of the philosophers opposite handed, we can eliminate deadlocks and ensure fairness! 
->This solution eliminates circular waiting and does not result in a deadlock under any situation. 
->Starvation may still occur, if some of the philosophers are quick eaters and thinkers.
->But this is avoided in our implementation since we use a random number generator to assign philosophers’ their eating and thinking time. 
->This is a trivial avoidance of the starvation problem. 
->We can maintain a global queue to detect and forcibly unblock these starving philosophers(although we still won’t be able to control how much they actually eat!).

General Thoughts on the Project:

->There was a substantial learning curve in reading and understanding how to implement signals, timers, context switches etc.
->And a compiled document of the required man pages would have been extremely useful.
->Joining Function does not perform as expected for complicated test cases.
->I am missing an edge case, but not sure if the problem is in the join function or a related function 
->(maybe in cancel function, when trying to join cancelled threads). Implementation of list was also straightforward. 
->A use of a more efficient data structure, maybe mapping all contexts using a hash maybe better, 
->especially if there are large number of threads(for constant lookup time).

