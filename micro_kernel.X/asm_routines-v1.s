
    ; NCP
    ; Timer 1 ISR with no C header
    
    .include "p33FJ128MC802.inc"
    
    .extern process_stack_address
    .extern temp_W0
    .extern new_process_stack_pointer
    .extern temp_stack_pointer
    .extern adr_current_process_stack_pointer
    
    .global _asmStackFrame
    ; Creates first stack frame of a process with current registers data
_asmStackFrame: 
    ; Save current stack pointer and W0 register
    MOV	    W15, _temp_stack_pointer
    MOV	    W0, _temp_W0
    
    ; Change stack to the process stack
    MOV	    _process_stack_address, W15
    
    ; Push process start address of code (high word address = 0000)
    MOV	    _process_code_address, W0
    PUSH    W0
    
    ; Push initial SR<7:0> IPL3 PC<22:16> = 0
    ; If another initial priority is needed then W0<15:13> must be used
    ; to contain initial IPL<2:0>
    MOV	    #0, W0
    PUSH    W0
    
    ; Push registers
    MOV	    _temp_W0, W0    ; Recover W0
    
    PUSH    SR
    PUSH    W0
    PUSH    W1
    PUSH.D  W2
    PUSH.D  W4
    PUSH.D  W6
    PUSH.D  W8
    PUSH.D  W10
    PUSH.D  W12
    PUSH    W14
    PUSH    RCOUNT
    PUSH    TBLPAG
    PUSH    ACCAL
    PUSH    ACCAH
    PUSH    ACCAU
    PUSH    ACCBL
    PUSH    ACCBH
    PUSH    ACCBU
    PUSH    DCOUNT
    PUSH    DOSTARTL
    PUSH    DOSTARTH
    PUSH    DOENDL
    PUSH    DOENDH
    PUSH    CORCON
    PUSH    PSVPAG    
    
    ; Comunicate process stack pointer
    MOV	    W15, _new_process_stack_pointer
    
    ; Recover initial stack pointer
    MOV	    _temp_stack_pointer, W15
    
    return
    
      
    ; This saves context of current running process, then call scheduller and
    ; prepare context of the selected process to run
__T1Interrupt:
    ; Return addres is in stack + low byte of SR + IPL3
    
    BCLR    IFS0, #T1IF	    ; Clears the interrupt flag
    PUSH    SR
    PUSH    W0
    PUSH    W1
    PUSH.D  W2
    PUSH.D  W4
    PUSH.D  W6
    PUSH.D  W8
    PUSH.D  W10
    PUSH.D  W12
    PUSH    W14
    PUSH    RCOUNT
    PUSH    TBLPAG
    PUSH    ACCAL
    PUSH    ACCAH
    PUSH    ACCAU
    PUSH    ACCBL
    PUSH    ACCBH
    PUSH    ACCBU
    PUSH    DCOUNT
    PUSH    DOSTARTL
    PUSH    DOSTARTH
    PUSH    DOENDL
    PUSH    DOENDH
    PUSH    CORCON
    PUSH    PSVPAG    
    
    ; Save old process stack pointer
    MOV	    _adr_current_process_stack_pointer, W0
    MOV	    W15, [W0]
    
    CALL    _scheduller
    
    ; Charge new process stack pointer    
    MOV	    _adr_current_process_stack_pointer, W0
    MOV	    [w0], W15
    
    POP	    PSVPAG    
    POP	    CORCON
    POP	    DOENDH
    POP	    DOENDL
    POP	    DOSTARTH
    POP	    DOSTARTL
    POP	    DCOUNT
    POP	    ACCBU
    POP	    ACCBH
    POP	    ACCBL
    POP	    ACCAU
    POP	    ACCAH
    POP	    ACCAL
    POP	    TBLPAG
    POP	    RCOUNT
    POP	    W14
    POP.D   W12
    POP.D   W10
    POP.D   W8
    POP.D   W6
    POP.D   W4
    POP.D   W2
    POP.D   W0
    POP	    SR

    RETFIE
        
    .end

