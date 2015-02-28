/* include necessary header files*/
#include <gtthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <ucontext.h>
#include <unistd.h>

/*define return values 0 and 1*/
#define value0 0
#define value1 1

/*Define true and false for use in setting values for alive, idle or dead threads*/

#define true 1
#define false 0

int Flag = 10;
int flag1 = 0;

struct context_linker *CONTEXTS;

/* Function to find if a node already exists in the data structure*/
int find_in_list(struct context_struct *remove_node)
{	/*declare a current node and a head node*/
	struct context_struct *current;
	struct context_struct *head;

	head = current = CONTEXTS->head;
/* Iterate through to find if element exists and return 1 if found*/
	for(; current != head; current = current->next)		
	{
		if(remove_node==(void*)current)			
		{
			return value1;
		}
	}
	/*Not Found*/
	return value0;
}

/* Function to add a new node todata structure*/
void add_to_list(struct context_struct *new_node)
{	/*First element*/
	if(CONTEXTS->head==NULL)
	{
		CONTEXTS->head=CONTEXTS->tail=CONTEXTS->current=new_node;
	}
	/*update list appropriately*/
	else
	{
		CONTEXTS->tail->next=new_node;
		new_node->next=CONTEXTS->head;
		CONTEXTS->tail=new_node;
	}
	CONTEXTS->tail->next=CONTEXTS->head;
}

/*Function to remove a node from the list(when thead is cancelled or over)*/
int remove_from_list(struct context_struct *thread)
{

	struct context_struct *remove_node=(struct context_struct *) thread;
	struct context_struct *current=CONTEXTS->head;
	
	#ifdef TRY
	temp_node=(struct context_struct *)thread =current;
	int hello= find_in_list(temp_node);
	printf( "\t Remove: found?  %d \n ", hello);
	
	#endif

	/*is it the last element?*/
	if(current==NULL)
	{
		return value0;
	}
	/*current element?*/
	if(current==remove_node)
	{
		CONTEXTS->head = CONTEXTS->head->next;	
	}
	/*traverse and update sructure if found*/
	else
	{
		while(current->next!=remove_node && current->next!=NULL)
		{
			current = current->next;
		}
		if(current!=remove_node && current->next==NULL)
		{
			return value0;
		}
		else
		{
			current->next=current->next->next;
		}
	}
	return value1;
}


static int thread1 = value0;


/* Function to handle signals and appropiately change thread state*/

void signals(int signal, siginfo_t *siginfo, void * context)
{
	struct context_struct *current;
	struct context_struct *next;

	if(signal == SIGVTALRM)
	{

		if(__sync_val_compare_and_swap(&thread1,0,1) == 0)
		{
			current = CONTEXTS->current;
			next = current->next;
			while(next->alive != true)
			{
				if(current == next && current->dead == true)
				{
					exit(0);
				}
				if(next->idle == true && next->existing_thread != NULL && next->existing_thread->dead == true)
				{

					next->alive = true;
					next->dead=next->idle=false;
				}
				else
				{
					next = next->next;
				}
			}

			
			CONTEXTS->current =  next;
			__sync_val_compare_and_swap(&thread1,1,0);
				swapcontext(&(current->context),&(next->context));
		}
		
	}
  
}
/* Function to create a new thread and see if the thread is actuallu dead*/

void new_thread(struct context_struct *entry)
{

	entry->ret_val = (entry->start_routine)(entry->arg);
	#ifdef TRY
		printf("\t New:Return %d \n", entry->ret_val);
	#endif
	__sync_val_compare_and_swap(&thread1,value0,value1);

	entry->dead = true;
	entry->idle=entry->alive=false;

	__sync_val_compare_and_swap(&thread1,value1,value0);
	gtthread_yield();
}



/*Gtthread function to create a new thread. Initializes required parameters and adds to list of contexts*/
int gtthread_create(gtthread_t *thread, void *(*start_routine)(void *),void *arg)
{
	struct context_struct *new_node;
	char *newstack;
	new_node=(struct context_struct *) malloc(sizeof(struct context_struct));
	newstack=(char *) malloc(MAX);

/*set states for the new thread*/
	new_node->alive = true;
	new_node->idle=new_node->dead=false;
	new_node->ret_val= NULL;
	new_node->start_routine=start_routine;
	new_node->arg=arg;

/*getcontext for the new node*/
	getcontext(&(new_node->context));

	new_node->context.uc_stack.ss_sp=newstack;
	new_node->context.uc_stack.ss_size=MAX;
	new_node->context.uc_link=NULL;

	makecontext(&(new_node->context),(void (*) (void)) new_thread,1,new_node);

	while(__sync_val_compare_and_swap(&thread1,value0,value1))
	{
		gtthread_yield();
	}

	new_node->thread_id = (CONTEXTS->thread_id)++;
/*add to contexts list*/
	add_to_list(new_node);

	*thread = new_node;
	__sync_val_compare_and_swap(&thread1,value1,value0);
	return 0;
}



/*Function to initialize the virtual timer and set it*/
void timing(long period)
{
	struct itimerval timer;
/*Add the usec values if granurality is required*/
	timer.it_value.tv_sec = period / 500000000L;
	timer.it_value.tv_usec = period % 500000000L;
	timer.it_interval.tv_sec = period / 500000000L;
	timer.it_interval.tv_usec = period % 500000000L;

	setitimer(ITIMER_VIRTUAL,&timer,NULL);
}



/*gtthread Function to initialize thread with a certain scheduled period*/
void gtthread_init(long period)
{
	struct context_struct *new_node;

	CONTEXTS = (struct context_linker *)malloc(sizeof(struct context_linker));
	CONTEXTS->head = CONTEXTS->tail = CONTEXTS->current = NULL;
	/*start assigning thread ids from say 1*/
	CONTEXTS->thread_id = 1;
	new_node = (struct context_struct *) malloc(sizeof(struct context_struct));
/*update state of new node and assign it a thread id*/
	new_node->alive = 1;
	new_node->idle=new_node->dead=0;
	new_node->ret_val = NULL;
	new_node->thread_id = (CONTEXTS->thread_id)++;
	
	#ifdef TRY
	temp_node=(struct context_struct *)thread = new_node;
	int hello= find_in_list(temp_node);
	printf( "\t Join: found?  %d \n ", hello);
	#endif
	
	#ifdef TRY
		printf("%d", new_node->thread_id);
	#endif
	
	add_to_list(new_node);
	
	#ifdef TRY
		printf("%d", new_node->thread_id);
	#endif
	
	/*use sigaction struct to set up required signals*/
	struct sigaction sa;
/*call signals function to assign sigaction. update flags and riase SIGVTALRM*/
	sa.sa_sigaction = signals;
	sa.sa_flags = SA_SIGINFO;
	sigemptyset(&sa.sa_mask);
	sigaction(SIGVTALRM, &sa, NULL);
/*call the timing function*/
	timing(period);

}


/*Gtthread function to yield the current thread*/
int gtthread_yield(void)
{	/*Just raise SIGVTALRM to preempt or end the thread!*/
	raise(SIGVTALRM);
	/*return 0*/
	return value0;
}


/*gtthread function to find id of calling thread*/
gtthread_t gtthread_self(void)
{	/*initialize return value*/
	gtthread_t retval = (gtthread_t) NULL;
	/*while thread1 is still 0, yield*/
	while(__sync_val_compare_and_swap(&thread1,value0,value1))
	gtthread_yield();
/*once out of the while loop, assign the current value as ret*/
	retval=(gtthread_t) CONTEXTS->current;
	
	#ifdef TRY
		printf("/t %d /n", retval);
	#endif
	
	__sync_val_compare_and_swap(&thread1,value1,value0);
	return retval;
}


/*Function to join a thread. Tricky!*/

int  gtthread_join(gtthread_t thread, void **status)
{
	/*declare pointers to type context_struct*/

	struct context_struct * current_thread;
	struct context_struct * current;
	struct context_struct * head;
	struct context_struct * remove_node;
	remove_node = (struct context_struct *)thread;

	while(__sync_val_compare_and_swap(&thread1,value0,value1)){gtthread_yield();}
	current_thread = CONTEXTS->current;
	
	#ifdef TRY
	temp_node=(struct context_struct *)thread;
	int hello= find_in_list(temp_node);
	printf( "\t Join: found?  %d \n ", hello);
	#endif
/*Atomic lock operation is thread1 is 1 when the node to be removed is current node*/
	if(remove_node == current_thread)
	{
		__sync_val_compare_and_swap(&thread1,value1,value0);
	}
	/* check if the node exists*/

	if(remove_node == NULL || find_in_list(remove_node) == 0)
	{
		__sync_val_compare_and_swap(&thread1,value1,value0);

	}
	/* waiting to join? */
	if(remove_node->existing_thread == current_thread)
	{
		__sync_val_compare_and_swap(&thread1,value1,value0);
	}
	/* Initialize to head to traverse the list */	
	head = current_thread;

/*iterate through to find our node and perform an atomic lock on it*/
	for(current = head->next; current != head; current = current->next)
	{
		if(current->existing_thread == remove_node)
		{	
			__sync_val_compare_and_swap(&thread1,value1,value0);

		}
	} 
/*   */
	if(remove_node->alive==true || remove_node->idle==true)
	{	/*update states*/
		current_thread->idle = true;
		current_thread->alive= current_thread->dead = false;
		current_thread->existing_thread = remove_node;
		__sync_val_compare_and_swap(&thread1,value1,value0);
		gtthread_yield();
		while(__sync_val_compare_and_swap(&thread1,value0,value1)){gtthread_yield();}
	}
	/* if status is not NULL, copy the return value into it*/
	if(status != NULL)
	{
		*status = remove_node->ret_val;
	}
/*remove context from list*/
	remove_from_list(remove_node);	
	if(remove_node->context.uc_stack.ss_sp != NULL)
	{
		free(remove_node->context.uc_stack.ss_sp);
	}
	/*free the removed node*/
	free(remove_node);
	__sync_val_compare_and_swap(&thread1,value1,value0);
/*return 0*/
	return 0;
}




/*Gtthread function to exit a trhead*/
void gtthread_exit(void *retval)
{
	struct context_struct *current_thread;
	current_thread = (struct context_struct *)gtthread_self();

	while(__sync_val_compare_and_swap(&thread1,value0,value1))
		{
			gtthread_yield();
		}
	/*update states*/
	current_thread->dead = true;
	current_thread->idle=current_thread->alive=false;
	/*update return value*/
	current_thread->ret_val = retval;

	#ifdef TRY
				printf("\t Exit: Return %d \n", current_thread->ret_val);
	#endif
	__sync_val_compare_and_swap(&thread1,value1,value0);
	gtthread_yield();

}



/*Gtthread function to see if two threads are equal*/
int  gtthread_equal(gtthread_t t1, gtthread_t t2)
{	
	int result;

	if(t1==t2)
	{	/*they are the same*/
		result=1;
	}
	else
	{	/*they are not the same*/
		result=0;
	}
	return result;
	#ifdef TRY
		printf("\t Equal : return \n", result);
	#endif
}



/*Gtthread function to cancel a thread*/
int  gtthread_cancel(gtthread_t thread)
{
	struct context_struct *remove_node;

	remove_node=(struct context_struct *)thread;

	while(__sync_val_compare_and_swap(&thread1,value0,value1))
	{
		gtthread_yield();
	}
	int result=find_in_list(remove_node);

	if(result==0)
	{
		__sync_val_compare_and_swap(&thread1,value1,value0);

	}

	remove_node->dead = true;
	remove_node->alive=remove_node->idle=false;

	remove_node->ret_val = GTTHREAD_CANCELED;

	__sync_val_compare_and_swap(&thread1,value1,value0);

	return value0;
}



/*Function to create a mutex*/
int  gtthread_mutex_init(gtthread_mutex_t *mutex)
{
	struct thread_mutex *temp;
	temp = malloc(sizeof(struct thread_mutex));

	temp->lock = value0;
	temp->owner = NULL;

	*mutex = temp;

	return value0;
}



/*gtthread function to lock a thread into mutex*/
int  gtthread_mutex_lock(gtthread_mutex_t *mutex)
{
	struct thread_mutex *temp;
	gtthread_t current_thread;
	temp = (struct thread_mutex *) *mutex;
	current_thread = gtthread_self();
	int result=gtthread_equal(temp->owner,current_thread);
	if(result == 1)
	{
		return value1;
	}

	while(__sync_val_compare_and_swap(&(temp->lock),value0,value1))
	{
		gtthread_yield();
	}

	temp->owner = current_thread;
	
	return value0;
}



/*Gtthread function to unlock mutex*/
int  gtthread_mutex_unlock(gtthread_mutex_t *mutex)
{
	struct thread_mutex *temp;
	/*check if mutex is NULL and return 1 if so*/                     
	if(mutex==NULL) 
	{
		return value1;
	}

	temp=(struct thread_mutex *) *mutex;
	gtthread_t current_thread=gtthread_self();
	/*check if the threads are equal*/
	int result= gtthread_equal(temp->owner, current_thread);
	
	if(result == 0)
	{/*return 1 if they are the same*/
		return value1;
	}

	temp->owner = NULL;
	__sync_val_compare_and_swap(&(temp->lock),value1,value0);
	return value0;
}




