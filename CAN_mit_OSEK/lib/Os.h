/**************************************************************************************************\
 * OSEK Betriebssystem für Microchip AVR Plattform. Der Timer/Counter 1-Baustein wird vom Betriebs-
 * system als "System Counter" benötigt. Weitere I/O-Bausteine werden nicht verwendet.
 *
 * Autor:   Prof. Dr. Axel Thuemmler
 * Datum:   19. Maerz 2023
 * Version: 1.1
 *
 * Funktionalitäten:
 * - OSEK API Funktionen zum Task Management, Event Control, Alarms und OS Execution Control.
 * - OSEK Alarm Entitäten mit Actionen ActivateTask, SetEvent und Callback.
 * - Unterstützung von Basic und Extended Tasks.
 * - Preemptives und nicht-preemptives Scheduling, preemptive Idle-Task.
 * - Tasks mit unterschiedlichen Prioritäten und mehrere Tasks pro Prioritätsstufe mit Scheduling
 *   in der Reihenfolge der Aktivierung.
 * - OSEK API Funktionen Fehlercheck mit Benachrichtigung via USART ein- / ausschaltbar.
 * - Prüfung auf Stack Korruption ein- / ausschaltbar.
 *
 * Restriktionen:
 * - OSEK API Funktionen zum Resource Management (TBD, comming soon) inkl. Priority Ceiling Protocol.
 * - Es werden neben dem System Counter keine weiteren Counter unterstützt, d.h. alle Alarms hängen
 *   am System Counter.
 * - Keine mehrfache Aktivierung der gleichen Task erlaubt.
 * - Keine OSEK Unterstützung für Interrupt Service Routinen der Kategorie 2 und keine Implementierung
 *   der OSEK API Funktionen für Interrupt handling (siehe OSEK Spezifikation).
 * - Keine Unterstützung von OSEK Hook-Funktionen (siehe OSEK Spezifikation).
 * - Nur ein Applikationsmodus (OSDEFAULTAPPMODE).
 *
 * Diese Software ist urheberrechtlich geschuetzt. Eine Vervielfaeltigung oder Verbreitung,
 * auch ausschnittsweise, ist nicht ohne die Zustimmung des Autors erlaubt.
\**************************************************************************************************/
#ifndef _OS_H_
#define _OS_H_

/*------------------------------------------------------------------------------------------------*/
/* DEFINES                                                                                        */
/*------------------------------------------------------------------------------------------------*/

/* Boolean values. */
#define FALSE 0
#define TRUE 1

/* Default USART baud rate used by the OS for logging and error messages, if USART not initialized before. */
#define OS_USART_DEFAULT_BAUD_RATE 115200

/* Error codes of OSEK-API services. */
#define E_OK 0
#define E_OS_ACCESS 1
#define E_OS_CALLEVEL 2
#define E_OS_ID 3
#define E_OS_LIMIT 4
#define E_OS_NOFUNC 5
#define E_OS_RESOURCE 6
#define E_OS_STATE 7
#define E_OS_VALUE 8

/* Constants of data type TaskStateType. */
#define RUNNING 0
#define WAITING 1
#define READY 2
#define SUSPENDED 3

/* Constant of data type TaskType for a not defined task. */
#define INVALID_TASK 0xff

/* Task types. */
#define BASIC_TASK 0
#define EXTENDED_TASK 1

/* Schuduling types. */
#define NON_PREEMPTIVE 0
#define PREEMPTIVE 1

/* Alarm types. */
#define ACTIVATETASK 0
#define SETEVENT 1
#define CALLBACK 2

/* Resource types. */
#define RES_SCHEDULER 0

/* OS default application mode. */
#define OSDEFAULTAPPMODE 0

/* Values of the system counter (attached to Timer/Counter 1). */
#define OSMAXALLOWEDVALUE 0xffffU   /* Maximum possible allowed value of the system counter in ticks, should not be greater than max. value of TickType. */
#define OSTICKSPERBASE 1            /* Number of ticks required to reach a specific unit (e.g. 10 milliseconds or 1 second) of the system counter. */
#define OSMINCYCLE 1                /* Minimum allowed number of ticks for a cyclic alarm of the system counter. */

/* Macro for task function. */
#define TASK(TaskName) void Func##TaskName(void)

/*------------------------------------------------------------------------------------------------*/
/* TYPE DEFINITIONS                                                                               */
/*------------------------------------------------------------------------------------------------*/

/* Datat type for return values (i.e. error codes) of OSEK system services. */
typedef unsigned char StatusType;

/* Data types for task management. */
typedef unsigned char TaskType;             /* Identifies a task. */
typedef TaskType* TaskRefType;              /* Points to a variable of type TaskType. */
typedef unsigned char TaskStateType;        /* Identifies the state of a task. */
typedef TaskStateType* TaskStateRefType;    /* Points to a variable of type TaskStateType. */

/* Data type for resource management. */
typedef unsigned char ResourceType;

/* Data types for event control. */
typedef unsigned char EventMaskType;        /* Data type of the event mask. */
typedef EventMaskType* EventMaskRefType;    /* Reference to an event mask. */

/* Data types for alarms. */
typedef unsigned int TickType;              /* Data type represents counter values in ticks. */
typedef TickType* TickRefType;              /* Points to a variable of type TickType. */
typedef struct                              /* Structure for storage of counter characteristics. */
{
    TickType maxallowedvalue;               /* Maximum possible allowed count value in ticks. */
    TickType ticksperbase;                  /* Number of ticks required to reach a counter-specific (significant) unit. */
    TickType mincycle;                      /* Smallest allowed value for the cycle-parameter of SetRelAlarm/SetAbsAlarm. */
} AlarmBaseType;
typedef AlarmBaseType* AlarmBaseRefType;    /* Points to a variable of type AlarmBaseType. */
typedef unsigned char AlarmType;            /* Identifies an alarm. */

/* Data type for application mode. */
typedef unsigned char AppModeType;

/* Data type for task pointer (i.e. void-void-pointer). */
typedef void (*TaskPtrT) (void);

/* Data type for constant task information. */
typedef struct
{
    unsigned char actOnStart;   /* TRUE = Task activated on StartOS(), FALSE = Task not activated on StartOS(). */
    unsigned char taskType;     /* BASIC_TASK or EXTENDED_TASK. */
    unsigned char priority;     /* Task priority 0 = lowest priority, 255 = highest priority. */
    unsigned char scheduling;   /* Task schedule type: NON_PREEMPTIVE or PREEMPTIVE. */
    unsigned char maxAct;       /* Maximum number of multiple task activations. */
    TaskPtrT taskPtr;
} TaskInfoBlockT;

/* Data type for variable task information. */
typedef struct
{
    unsigned long activationOrder;      /* Activation time stamp for tasks in ready-queue with same priority. */
    unsigned char* stack;               /* Pointer to task stack memory start. */
    unsigned char* stackPtr;            /* Pointer to first free stack position.  */
    TaskStateType state;                /* Current state of the task, i.e. RUNNING, READY, SUSPENDED or WAITING. */
    EventMaskType eventMask;            /* Event mask with occurred events. */
    EventMaskType eventMaskWaiting;     /* Event mask with events the task is waiting for (in state WAITING). */
} TaskControlBlockT;

/* Data type for constant alarm information. */
typedef struct 
{
    unsigned char action;       /* Alarm action: ACTIVATETASK, SETEVENT or CALLBACK. */ 
    TaskType task;              /* Task ID for alarm actions ACTIVATETASK and SETEVENT. */
    EventMaskType mask;         /* Event mask for alarm action SETEVENT. */
    void (*callback)();         /* Void-void-Callback function for alarm action CALLBACK. */
} AlarmInfoBlockT;

/* Data type for variable alarm information. */
typedef struct
{
    TickType nextOccurrence;    /* Absolut value of system counter when the alarms occurres next. */
    TickType cycle;             /* Counter ticks for cyclic alarms. */
    unsigned char inUse;        /* Boolean with TRUE or FALSE. */
} AlarmControlBlockT;

/* Data type for complete OSEK configuartion. */
typedef struct
{
    unsigned int tickDuration;      /* Duration of the system timer until next interrupt with a system counter increment. */
    unsigned char stackSize;        /* Stack size in bytes per task. */
    unsigned char numberOfTasks;    /* Overall number of tasks including the idle task. */
    unsigned char numberOfAlarms;   /* Overall number of alarms. */
    unsigned char checkStackCorruption;
    unsigned char messageOnApiServiceError;
    unsigned char stopOnApiServiceError;
} OsConfigT;

/*------------------------------------------------------------------------------------------------*/
/* OSEK SYSTEM SERVICES                                                                           */
/*------------------------------------------------------------------------------------------------*/

/* OSEK system services: Task management. */
StatusType ActivateTask(TaskType taskId);
StatusType TerminateTask();
StatusType ChainTask(TaskType taskId);
StatusType Schedule();
StatusType GetTaskID(TaskRefType taskId);
StatusType GetTaskState(TaskType taskId, TaskStateRefType state);

/* OSEK system services: Resource management. */
StatusType GetResource(ResourceType resId);
StatusType ReleaseResource(ResourceType resId);

/* OSEK system services: Event control. */
StatusType SetEvent(TaskType taskId, EventMaskType mask);
StatusType ClearEvent(EventMaskType mask);
StatusType GetEvent(TaskType taskId, EventMaskRefType mask);
StatusType WaitEvent(EventMaskType mask);

/* OSEK system services: Alarms. */
StatusType GetAlarmBase(AlarmType alarmId, AlarmBaseRefType info);
StatusType GetAlarm(AlarmType alarmId, TickRefType tick);
StatusType SetRelAlarm(AlarmType alarmId, TickType increment, TickType cycle);
StatusType SetAbsAlarm(AlarmType alarmId, TickType start, TickType cycle);
StatusType CancelAlarm(AlarmType alarmId);

/* OSEK system services: Operating system execution control. */
AppModeType GetActiveApplicationMode();
void StartOS(AppModeType);              /* Different application modes are not supported right now. Use OSDEFAULTAPPMODE. */
void ShutdownOS(StatusType error);

/*------------------------------------------------------------------------------------------------*/
/* PROPRIETARY FUNCTIONS                                                                          */
/*------------------------------------------------------------------------------------------------*/

TickType Os_GetSytemCounter();

#endif /* _OS_H_ */