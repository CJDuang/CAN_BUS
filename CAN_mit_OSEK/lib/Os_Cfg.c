/**************************************************************************************************\
 * OSEK Betriebssystem für Microchip AVR Plattform. Konfigurationsspezifische Implementierung.
 * Dieser Teil des Betriebssystems muss zusammen mit der Applikation, welche das OSEK nutzt, 
 * übersetzt werden.
 *
 * Autor:   Prof. Dr. Axel Thuemmler
 * Datum:   19. Maerz 2023
 * Version: 1.1
 *
 * Diese Software ist urheberrechtlich geschuetzt. Eine Vervielfaeltigung oder Verbreitung,
 * auch ausschnittsweise, ist nicht ohne die Zustimmung des Autors erlaubt.
\**************************************************************************************************/

/* Constant task information blocks (IB). */
const TaskInfoBlockT OsTaskIB[NUMBER_OF_TASKS] = OS_TASK_INFO_BLOCK;

/* Task control blocks (CB) with runtime-data for the OS. */
TaskControlBlockT OsTaskCB[NUMBER_OF_TASKS];

/* Buffer for all stacks including two start and stop bytes for protection against stack corruption. */
unsigned char OsTaskSB[(OS_STACK_SIZE_PER_TASK + 2) * NUMBER_OF_TASKS + 2];

/* Constant alarm information blocks (IB). */
const AlarmInfoBlockT OsAlarmIB[NUMBER_OF_ALARMS] = OS_ALARM_INFO_BLOCK;

/* Alarm control blocks (CB) with runtime-data for the OS. */
AlarmControlBlockT OsAlarmCB[NUMBER_OF_ALARMS];

/* OS configuration data. */
OsConfigT OsCfg = { OSTICKDURATION, OS_STACK_SIZE_PER_TASK, NUMBER_OF_TASKS, NUMBER_OF_ALARMS,
            OS_CHECK_STACK_CORRUPTION, OS_MESSAGE_ON_API_SERVICE_ERROR, OS_STOP_ON_API_SERVICE_ERROR };
