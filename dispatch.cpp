#include "dispatch.h"

void *dispatch(void *d) {
	TaskInfo *curr_task = nullptr;
	pthread_t thread;

	// Wait until a task finishes and then move to next
	while (1) {
		if (sched->Task_Count() > 0) {	
			// Decrement the semaphore
			sem_wait(t_sem);
			// Run the new task
			curr_task = sched->Next_Task();
			// Now that we are running thread, allow new connection
			sem_post(a_sem);
			pthread_create(&thread, 
					NULL, 
					DoRequest, 
					(void *) curr_task);
		}

	}

	return nullptr;
}
