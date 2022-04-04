#ifndef	SCHEDULER_FACTORY_H
#define	SCHEDULER_FACTORY_H
#include <cstring>
#include "scheduler_FIFO.h"

extern Scheduler *sched;

Scheduler *MakeScheduler(const char *scheduler);

#endif
