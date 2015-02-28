#ifndef __GTTHREAD_H
#define __GTTHREAD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <ucontext.h>
#include <errno.h>
#include <unistd.h>

typedef void * gtthread_t;
typedef void * gtthread_mutex_t;

#define MAX 60000


struct context_struct
{
	int alive;
	int dead;
	int idle;
	void *(*start_routine)(void *);
	void *ret_val;
	void *arg;
	int thread_id;
	ucontext_t context;
	struct context_struct *next;
	struct context_struct *existing_thread; 
};


struct context_linker
{	
	int thread_id;
	struct context_struct *head;
	struct context_struct *tail;
	struct context_struct *current;
	int active;
	int flag;
};

struct thread_mutex
{
	int lock;
	gtthread_t owner;
};

#define GTTHREAD_CANCELED ((void*)-1)


void gtthread_init(long period);

int  gtthread_create(gtthread_t *thread, void *(*start_routine)(void *), void *arg);

int  gtthread_join(gtthread_t thread, void **status);

void gtthread_exit(void *retval);

int gtthread_yield(void);

int  gtthread_equal(gtthread_t t1, gtthread_t t2);

int  gtthread_cancel(gtthread_t thread);

gtthread_t gtthread_self(void);

int  gtthread_mutex_init(gtthread_mutex_t *mutex);

int  gtthread_mutex_lock(gtthread_mutex_t *mutex);

int  gtthread_mutex_unlock(gtthread_mutex_t *mutex);


#endif /* __GTTHREAD_H */