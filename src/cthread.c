
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../include/support.h"
#include "../include/cthread.h"
#include "../include/cdata.h"

PFILA2 running_q = NULL;
PFILA2 ready_high_q = NULL;
PFILA2 ready_average_q = NULL;
PFILA2 ready_low_q = NULL;
PFILA2 blocked_q = NULL;
PFILA2 sem_q = NULL;
ucontext_t scheduler_context;

int initialized = 0;
int t_id = 0;
int stack_size = 16384;

void initialize(void);
void schedule(void);
void add_to_fila(TCB_t* tcb);
void remove_from_fila(TCB_t* tcb);
int fila_empty(PFILA2 fila);
int create_context(ucontext_t* context, void(*func)(), void* arg, int link_to_scheduler);
TCB_t* create_tcb(int id, int state, int prio, ucontext_t context);
TCB_t* find_tcb_in_fila(PFILA2 fila, int id);
TCB_t* find_tcb(int id);

int ccreate (void* (*start)(void*), void *arg, int prio) {
	int ret = -1;
	initialize();
	if(initialized == 0)
	{
		printf("Not possible to initialize cthread lib");
		return ret;
	}

	ucontext_t context;
	int success = create_context(&context, (void(*)(void))start, arg, 1);
	if(!success)
	{
		return ret;
	}

	TCB_t* tcb = create_tcb(++t_id, PROCST_APTO, prio, context);
	if(tcb)
	{
		add_to_fila(tcb);
		ret = tcb->tid;
	}
	return ret;
}

int csetprio(int tid, int prio) {
	return -1;
}

int cyield(void) {
	return -1;
}

int cjoin(int tid) 
{
	TCB_t* tcb;
	TCB_t* joining_tcb;
	
	if(initialized == 0)
	{
		initialize();
	}

	tcb = find_tcb(tid);
	if(!tcb)
	{
		return -1;
	}

	FirstFila2(running_q);
	joining_tcb = GetAtIteratorFila2(running_q);	
	
	remove_from_fila(joining_tcb);
	remove_from_fila(tcb);

	joining_tcb->state = PROCST_BLOQ;
	tcb->state = PROCST_EXEC;

	add_to_fila(joining_tcb);
	add_to_fila(tcb);

	swapcontext(&joining_tcb->context, &tcb->context);
	
	return 0;
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

void schedule(void)
{
	printf("Estou na schedule");
}

void initialize(void)
{
	if(initialized != 0)
	{
		return;
	}
	running_q = (PFILA2)malloc(sizeof(FILA2));
	CreateFila2(running_q);

	ready_high_q = (PFILA2)malloc(sizeof(FILA2));
	CreateFila2(ready_high_q);

	ready_average_q = (PFILA2)malloc(sizeof(FILA2));
	CreateFila2(ready_average_q);
		
	ready_low_q = (PFILA2)malloc(sizeof(FILA2));
	CreateFila2(ready_low_q);

	blocked_q = (PFILA2)malloc(sizeof(FILA2));
	CreateFila2(blocked_q);
		
	sem_q = (PFILA2)malloc(sizeof(FILA2));
	CreateFila2(sem_q);

	create_context(&scheduler_context, schedule, NULL, 0);

	ucontext_t main_context;
	getcontext(&main_context);
	TCB_t* main_tcb = create_tcb(0, PROCST_EXEC, 0, main_context);
	add_to_fila(main_tcb);

	initialized = 1;
}

int fila_empty(PFILA2 fila)
{	
	FirstFila2(fila);
	TCB_t* ptr = (TCB_t*)GetAtIteratorFila2(fila);

	if(ptr == NULL)
	{
		return 1;
	}
	return 0;
}

void add_to_fila(TCB_t* tcb)
{
	switch(tcb->state)
	{
		case PROCST_APTO:
		switch(tcb->prio)
		{
			case 2:
			AppendFila2(ready_high_q, (void*)tcb);
			break;
			case 1:
			AppendFila2(ready_average_q, (void*)tcb);
			break;
			case 0:
			AppendFila2(ready_low_q, (void*)tcb);
			break; 
		}
		break;
		case PROCST_BLOQ:
		AppendFila2(blocked_q, (void*)tcb);
		break;
		case PROCST_EXEC:
		AppendFila2(running_q, (void*)tcb);;
		break;
	}
}

void remove_from_fila(TCB_t* tcb)
{
	if(!find_tcb(tcb->tid))
	{
		return;
	}

	switch(tcb->state)
	{
		case PROCST_APTO:
		switch(tcb->prio)
		{
			case 2:
			DeleteAtIteratorFila2(ready_high_q);
			break;
			case 1:
			DeleteAtIteratorFila2(ready_average_q);
			break;
			case 0:
			default:
			DeleteAtIteratorFila2(ready_low_q);
			break; 
		}
		break;
		case PROCST_BLOQ:
		DeleteAtIteratorFila2(blocked_q);
		break;
		case PROCST_EXEC:
		DeleteAtIteratorFila2(running_q);
		break;
	}
}

int create_context(ucontext_t* context, void(*func)(), void* arg, int link_to_scheduler)
{
	int ret = 0;

	getcontext(context);
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

	context->uc_stack.ss_sp = ss_sp;
	context->uc_stack.ss_size = stack_size;
	context->uc_link = link_to_scheduler ? &scheduler_context : NULL;

	int arg_size = arg != NULL ? 1 : 0;
	makecontext(context, func, arg_size, arg);
	
	if(errno != 0)
	{
		printf("Error in makecontext.: %d", errno);
		return ret;
	}

	ret = 1;
	return ret;
}

TCB_t* create_tcb(int id, int state, int prio, ucontext_t context)
{
	TCB_t* tcb = (TCB_t*)malloc(sizeof(TCB_t));
	if(tcb == NULL)
	{
		printf("\nNot enough storage for TCB\n");
	}
	else
	{
		tcb->tid = id;
		tcb->state = state;
		tcb->prio = prio;
		tcb->context = context;
	}
	return tcb;
}

TCB_t* find_tcb_in_fila(PFILA2 fila, int id)
{
	TCB_t* ret = NULL;
	if(fila && !fila_empty(fila))
	{
		FirstFila2(fila);
		do
		{
			TCB_t* tcb = (TCB_t*)GetAtIteratorFila2(fila);
			if(tcb->tid == id)
			{
				ret = tcb;
				break;
			}
		}
		while(NextFila2(fila) == 0);
	}
	return ret;
}

TCB_t* find_tcb(int id)
{
	TCB_t* tcb;
	
	tcb = find_tcb_in_fila(ready_high_q, id);
	if(!tcb)
	{
		tcb = find_tcb_in_fila(ready_average_q, id);
	}

	if(!tcb)
	{
		tcb = find_tcb_in_fila(ready_low_q, id);
	}

	if(!tcb)
	{
		tcb = find_tcb_in_fila(blocked_q, id);
	}
	return tcb;
}