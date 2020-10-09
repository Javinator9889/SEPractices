
/*-----------------------------------------------------------------
 AuK variables and routines
 ------------------------------------------------------------------*/

#include "xc.h"
#include "AuK_v1_0.h"

#define max_TCB_indx 10

void init_tmr1(void)
{
    TMR1 = 0;    
    //PR1 = 15480; /* Una interrupción cada 100 ms */
    PR1 = 1548; /* Una interrupción cada 10 ms */
    //PR1 = 155; /* Una interrupción cada ms (aprox) */

    T1CONbits.TSIDL = 1; /* Stop on idle mode */
    T1CONbits.TGATE = 0; /* Internal clock source */
    T1CONbits.TCS = 0;
    T1CONbits.TCKPS = 3; /* 256 prescaler */

    IPC0bits.T1IP = 7; /* Priority level */
    IFS0bits.T1IF = 0; /* Clear interrupt flag */
    IEC0bits.T1IE = 0; /* Enable T1 interrupts */

    T1CONbits.TON = 0; /* Start timer */
}



unsigned int mem[2000];  // Word array for Task stacks
unsigned int indx_free_mem = 0;

/* Task states */
enum T_state{running, blocked, ready};

/* Task control block */
struct TCB
{
    int stack_pointer;
    int state;
    char in_use;
};

/* Task descriptor.
 Last slot (max_TCB_indx) will be used for Idle process. */
struct TCB TCB_Descriptor[max_TCB_indx + 1];

/* Maneuver variables used in asm code */

unsigned int temp_stack_pointer;
unsigned int temp_W0;

unsigned int process_stack_address;
unsigned int new_process_stack_pointer;
unsigned int process_code_address;

unsigned int adr_current_process_stack_pointer;

/* Could be used if it is necessary to return from AuK */
unsigned int system_stack_pointer;


/* Task management variables */
int current_TCB_indx;

/* Asm functions */
extern void asmStackFrame(void);


/* Return address of the memory space whith the specified words */
int GiveMeMemory(int num_words)
{
    int return_address = (unsigned int)&mem[indx_free_mem];
    
    indx_free_mem += num_words;
    
    return(return_address);
}



void init_task_manager()
{
    int x;
    for (x=0; x<= max_TCB_indx; x++)
    {
        TCB_Descriptor[x].stack_pointer = 0;
        TCB_Descriptor[x].state = blocked;
        TCB_Descriptor[x].in_use = FALSE;
    }
    
    init_tmr1();
}

char In_HCR = FALSE; /* In hard critical region */
char Old_IPL;

void enter_HCR(void)
{
    //Old_IPL = SRbits.IPL;
    //SRbits.IPL = 7;
    In_HCR = TRUE;
    //SRbits.IPL = Old_IPL;
}

void exit_HCR(void)
{
    //Old_IPL = SRbits.IPL;
    //SRbits.IPL = 7;
    In_HCR = FALSE;
    //SRbits.IPL = Old_IPL;
}

int give_me_my_id(void)
{
    return(current_TCB_indx);
}

/* Set adr_new_process_stack_pointer to the new active process */
void scheduller()
{
    int TCB_indx, indx_count;
    
    if(!In_HCR)
    {
        if (TCB_Descriptor[current_TCB_indx].state == running)
            TCB_Descriptor[current_TCB_indx].state = ready;
        /* If idle process was running TCB_indx will be 1 instead 0 after the 
        following assign, but it doesn't matter becouse in while loop 
         TCB_indx = 0 will be reached if necessary */
        TCB_indx = (current_TCB_indx +1) % max_TCB_indx;
        indx_count = 1;
    
        while((indx_count <= max_TCB_indx) && 
                (TCB_Descriptor[TCB_indx].state != ready))
        {
            TCB_indx = (TCB_indx +1) % max_TCB_indx;
            indx_count++;
        }
    
        if (TCB_Descriptor[TCB_indx].state != ready)
            /* There's no ready process, then select idle process */
            TCB_indx = max_TCB_indx;
        
        TCB_Descriptor[TCB_indx].state = running;
    
        adr_current_process_stack_pointer = 
                (unsigned int)&TCB_Descriptor[TCB_indx].stack_pointer;
        current_TCB_indx = TCB_indx;
    }
}

/*-------------------------------------*/
/* code_address must contain address of the new task code.
 * mem_needed must contain  the nedded words for the process stack */
int create_task(unsigned int code_address, unsigned int mem_needed)
{
    int TCB_indx;
    
    /* Find a free TCB */
    TCB_indx = 0;
    while(TCB_Descriptor[TCB_indx].in_use && TCB_indx < max_TCB_indx)
        TCB_indx++;
    
    if (TCB_indx == max_TCB_indx) // max_TCB_indx is reserved for idle process
        /* There is no free TCB */
        return(-1);
    
    process_stack_address = GiveMeMemory(mem_needed);
    process_code_address = code_address;
    
    /* Create new process stack frame */
    asmStackFrame();
    
    /* Save new process Stack pointer from asm routine */
    TCB_Descriptor[TCB_indx].stack_pointer = new_process_stack_pointer;
    TCB_Descriptor[TCB_indx].state = ready;
    TCB_Descriptor[TCB_indx].in_use = TRUE;
    return(TCB_indx);
}



void Idle_process(void)
{
    current_TCB_indx = max_TCB_indx; // Idle process
    TCB_Descriptor[current_TCB_indx].stack_pointer = GiveMeMemory(150);
    TCB_Descriptor[current_TCB_indx].state = running;
    TCB_Descriptor[current_TCB_indx].in_use = TRUE;
    
    /* Change to idle process stack */
    system_stack_pointer = WREG15;
    WREG15 = TCB_Descriptor[current_TCB_indx].stack_pointer;
    adr_current_process_stack_pointer = 
            (unsigned int) &TCB_Descriptor[current_TCB_indx].stack_pointer;
    
    /* Activate timer 1 interrupt */
    IEC0bits.T1IE = 1; /* Enable T1 interrupts */
    T1CONbits.TON = 1; /* Start timer */
    
    while(1);
}


/*-----------------------------------------------------------------
 End of AuK variables and routines
 ------------------------------------------------------------------*/

