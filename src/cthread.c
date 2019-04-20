
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../include/support.h"
#include "../include/cthread.h"
#include "../include/cdata.h"

int t_id = 0;
int stack_size = 16384;

int ccreate (void* (*start)(void*), void *arg, int prio) {
	int ret = -1;
	ucontext_t context;

	getcontext(&context);
	if(errno != 0)
	{
		printf("Error in getcontext.: %d", errno);
		return ret;
	}
	
	char* ss_sp = (char*)malloc(stack_size * sizeof(char));
	if(ss_sp == NULL)
	{
		printf("Not enough storage for stack");
		return ret;
	}

	context.uc_stack.ss_sp = ss_sp;
	context.uc_stack.ss_size = stack_size;
	context.uc_link = 0;
	makecontext(&context, (void(*)(void))start, 1, arg);
	
	if(errno != 0)
	{
		printf("Error in makecontext.: %d", errno);
		return ret;
	}	
	
	TCB_t* tcb = (TCB_t*)malloc(sizeof(TCB_t));
	tcb->tid = ++t_id;
	tcb->state = PROCST_APTO;
	tcb->prio = prio;
	tcb->context = context;

	ret = tcb->tid;
	free(tcb->context.uc_stack.ss_sp);
	free(tcb);
		
	return ret;
}

int csetprio(int tid, int prio) {
	return -1;
}

int cyield(void) {
	return -1;
}

int cjoin(int tid) {
	return -1;
}

int csem_init(csem_t *sem, int count) {
	return -1;
}

int cwait(csem_t *sem) {
	return -1;
}

int csignal(csem_t *sem) {
	return -1;
}

int cidentify (char *name, int size) {
	strncpy (name, "\nCristian Silva Grosseli - 00243693\nGabriel Simonetti Souza - 00243661\n", size);
	return 0;
}


