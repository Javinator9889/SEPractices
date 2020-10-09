/* 
 * File:   AuK_v1_0.h
 * Author: norberto
 *
 * Created on 14 de diciembre de 2018, 17:13
 */

#ifndef AUK_V1_0_H
#define	AUK_V1_0_H

#define TRUE 1
#define FALSE 0

/* TMR1 is used inside AuK, then it could not be used by system 
 * to be executed */

/* This routine will be called before using any AuK service */
void init_task_manager();

/* code_address must contain address of the new task code.
 * mem_needed must contain  the nedded words for the process stack */
int create_task(unsigned int code_address, unsigned int mem_needed);

/* Returns the task identifier */
int give_me_my_id(void);

/* This routine will be called when it is needed AuK start working.
 As its name suggests, this routine also contains the idle process code. */
void Idle_process(void);

/* Enter hard critical region */
void enter_HCR(void);

/* Exit hard critical region */
void exit_HCR(void);


#endif	/* AUK_V1_0_H */

