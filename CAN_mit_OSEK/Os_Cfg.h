#ifndef _OS_CFG_H_
#define _OS_CFG_H_

#include "Os.h"

/*------------------------------------------------------------------------------------------------*/
/* DEFINES                                                                                        */
/*------------------------------------------------------------------------------------------------*/

/* Duration of a tick of the system counter (Timer/Counter 1) in milliseconds. */
#define OSTICKDURATION 10

/* Stack size per task in bytes. */
#define OS_STACK_SIZE_PER_TASK 80

/* If TRUE then stack corruption ist checked during OS execution upon leaving and calling a task. */
#define OS_CHECK_STACK_CORRUPTION TRUE

/* If TRUE the OS sends an USART message if an API service returns an error. */
#define OS_MESSAGE_ON_API_SERVICE_ERROR TRUE

/* If TRUE the OS immediately stops after sending an USART message (if OS_MESSAGE_ON_API_SERVICE_ERROR is TRUE) if an API service returns an error. */
#define OS_STOP_ON_API_SERVICE_ERROR TRUE

/* Overall number of OS tasks. */
#define NUMBER_OF_TASKS 4

/* Overall number of OS alarms. */
#define NUMBER_OF_ALARMS 2

/* Definition of task IDs (= indices in task control block). */
#define IdleTask    0
#define StartUpTask 1
#define Task1       2
#define Task2       3

/* Definition of alarm IDs. */
#define Alarm1 0
#define Alarm2 1

/* Task info block. */
#define OS_TASK_INFO_BLOCK \
{ \
	{ /* IdleTask */ \
		TRUE,                 /* TRUE since Idle-Task must be activated during StartOS(). */ \
		BASIC_TASK,           /* Idle-Task can be BASIC_TASK or EXTENDED_TASK. */  \
		0,                    /* Idle-Task should have the lowest priority, i.e. 0. */ \
		PREEMPTIVE,           /* Idle-Task must be PREEMPTIVE. */ \
		1,                    /* Maximum number of multiple task activations should be 1. */ \
		FuncIdleTask          /* Task function. */ \
	}, \
	{ /* StartupTask */ \
		TRUE,                 /* TRUE = Task activated on StartOS(), FALSE = Task not activated on StartOS(). */ \
		BASIC_TASK,           /* BASIC_TASK or EXTENDED_TASK. */  \
		100,                  /* Task priority 0 = lowest priority, 255 = highest priority. */ \
		NON_PREEMPTIVE,       /* Task schedule type: NON_PREEMPTIVE or PREEMPTIVE. */ \
		1,                    /* Maximum number of multiple task activations. */ \
		FuncStartUpTask       /* Task function. */ \
	}, \
	{ /* Task 1 */ \
		FALSE,                /* TRUE = Task activated on StartOS(), FALSE = Task not activated on StartOS(). */ \
		BASIC_TASK,           /* BASIC_TASK or EXTENDED_TASK. */  \
		5,                    /* Task priority 0 = lowest priority, 255 = highest priority. */ \
		NON_PREEMPTIVE,       /* Task schedule type: NON_PREEMPTIVE or PREEMPTIVE. */ \
		1,                    /* Maximum number of multiple task activations. */ \
		FuncTask1             /* Task function. */ \
	}, \
	{ /* Task 2 */ \
		FALSE,                /* TRUE = Task activated on StartOS(), FALSE = Task not activated on StartOS(). */ \
		BASIC_TASK,           /* BASIC_TASK or EXTENDED_TASK. */  \
		10,                   /* Task priority 0 = lowest priority, 255 = highest priority. */ \
		NON_PREEMPTIVE,       /* Task schedule type: NON_PREEMPTIVE or PREEMPTIVE. */ \
		1,                    /* Maximum number of multiple task activations. */ \
		FuncTask2             /* Task function. */ \
	} \
}

/* Alarm info block. */
#define OS_ALARM_INFO_BLOCK \
{ \
	{ /* Alarm1 */ \
		ACTIVATETASK,         /* Alarm action: ACTIVATETASK, SETEVENT or CALLBACK. */  \
		Task1,                /* Task ID for alarm action ACTIVATETASK and SETEVENT. */  \
		0,                    /* Event mask for alarm action SETEVENT. */ \
		0                     /* Void-void-Callback function for alarm action CALLBACK. */ \
	}, \
	{ /* Alarm2 */ \
		ACTIVATETASK,         /* Alarm action: ACTIVATETASK, SETEVENT or CALLBACK. */  \
		Task2,                /* Task ID for alarm action ACTIVATETASK and SETEVENT. */  \
		0,                    /* Event mask for alarm action SETEVENT. */ \
		0                     /* Void-void-Callback function for alarm action CALLBACK. */ \
	} \
}

/*------------------------------------------------------------------------------------------------*/
/* TASK FUNCTION PROTOTYPES                                                                       */
/*------------------------------------------------------------------------------------------------*/

/* OS tasks forward declarations. */
extern void FuncIdleTask();
extern void FuncStartUpTask();
extern void FuncTask1();
extern void FuncTask2();

/*------------------------------------------------------------------------------------------------*/
/* CONFIGURATION-SPECIFIC OS IMPLEMENTATION                                                       */
/*------------------------------------------------------------------------------------------------*/

/* Include of the configuration-specific implemention of the OS. */
#include "Os_Cfg.c"

#endif /* _OS_CFG_H_ */

