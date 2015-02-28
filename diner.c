#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "gtthread.h"

#define N 5

/*declare a structure to hold philosophers names and preference for chopsticks*/
typedef struct philosopher
{
	const char *name;
	gtthread_mutex_t *left_chopstick;
	gtthread_mutex_t *right_chopstick;
};

/*Function to feed the philosophers. */
void eat(struct philosopher *phillo)
{
		int time1;
		printf("%s is eating.\n",phillo->name);
		/*take a random number. Change the number below to find out how long they eat*/
		time1 = (rand()%1000000 + 500000);
		while(time1>0)
		{
			gtthread_yield();
			time1=time1-10;

		}
	/*unclock both chopsticks and let the other philosophers eat*/
		gtthread_mutex_unlock(phillo->left_chopstick);

		gtthread_mutex_unlock(phillo->right_chopstick);
}

void think(struct philosopher *phillo)
{
	int time2;
	printf("%s is thinking.\n",phillo->name);
	/*Let the philosophers think for a random time. Change number below to find out how long they think*/
	time2 = (rand()%1000000 + 500000);

		while(time2>0)
		{
			gtthread_yield();
			time2=time2-10;

		}

	gtthread_mutex_lock(phillo->left_chopstick);

	gtthread_mutex_lock(phillo->right_chopstick);
}

void * run(void *arg)
{
	struct philosopher *philly = (struct philosopher *)arg;

	while(1)
	{
		/*think and eat in all glory till end of time*/
		think(philly);
		eat(philly);
		
	}
}
	


int main(int argc, char **argv)
{
	int i;

	struct philosopher philosophers[N];
	
	gtthread_mutex_t chops[N];	
	
	gtthread_t threads[N];
	
	gtthread_init(50000L);
		
	for(i = 0; i < N; ++i)
	{
		gtthread_mutex_init(&(chops[i]));
	}

for(i =0; i<N; i++)
switch(i)
{
	case 0:
	{
	philosophers[0].name = "Gandhi";
	}
	case 1:
	{
	philosophers[1].name = "Shankara";
	}
	case 2:
	{
	philosophers[2].name = "Vishwamitra";
	}
case 4:
	{
	philosophers[3].name = "Chanakya";

	}
case 5:
	{
	philosophers[4].name = "Aurobindo";
	}
}
	
for( i=0;i<4;i++)
{
	philosophers[i].left_chopstick=&(chops[i]);
	philosophers[i].right_chopstick=&(chops[i+1]);

}
/* Notice here that we make Aurobindo opposite handed to the other four philosophers!*/

philosophers[4].left_chopstick = &(chops[0]);
philosophers[4].right_chopstick = &(chops[4]);

	for(i = 0; i < 5; ++i)
	{
		gtthread_create(&(threads[i]),run,&(philosophers[i]));
	}

	gtthread_exit(NULL);
	return 0;
}