#include "pragmas.h"
#include <xc.h>
#include <libpic30.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "AuK_v1_0.h"
#include "init.h"
// The amount of both philosophers and forks is variable
// up-to ?N?, not limited to 5
#define N_PH    5

// Helper function for taking the maximum and minimum value
#define max(a, b) ((a < b) ? b : a)
#define min(a, b) ((a < b) ? a : b)

// This is not neccessary indeed as C manages int as bool,
// but for safety reasons use different arrays.
// Use additional option: -std=gnu99
#ifdef HIERARCHY_ALG
int forks[N_PH] = { [0 ... (N_PH - 1)] = -1 };
#else
bool forks[N_PH] = { [0 ... (N_PH - 1)] = false };
#endif
typedef enum ph_state{Thinking, Eating, Sleeping} ph_state;

int mod(int a, int b)
{
    int m = a % b;
    
    return (m < 0) ? (m + abs(b)) : m;
}

void philosopher() {
    // Save and store who we are
    const int ID = give_me_my_id();

    // The philospher always starts thinkig.
    // An improvement consist in starting at a random initial state
    ph_state state = Thinking;
    while (true) {
        switch (state) {
            case Eating:
            {
                // The philosopher takes the fork to his left
                int fork1 = mod(ID - 1, 5);
                // and the fork to his right
                int fork2 = ID;
                bool eating = false;

                // Enter critical section to check if forks
                // are available
                enter_HCR();
                if (!forks[fork1] && !forks[fork2]) {
                    // Take the two forks and start eating
                    forks[fork1] = true;
                    forks[fork2] = true;
                    eating = true;
                }
                exit_HCR();

                // Simulation of the philosopher eating...
                if (eating) {
                    enter_HCR();
                    printf("Ph %d is eating\n", ID);
                    exit_HCR();

                    // The eating process - an improvement could be
                    // setting a random delay time
                    __delay32(396288UL);

                    enter_HCR();
                    // Finally, leave both forks and transit to next state (sleep)
                    forks[fork1] = false;
                    forks[fork2] = false;
                    exit_HCR();

                    state = Sleeping;
                }
                break;
            }
                // Actually, 'Thinking' and 'Sleeping' do the same:
                // print a message and delay for an arbitrary time,
                // so we can handle both of them in the same case
                // definition
            case Thinking:
            case Sleeping:
            {
                // Inform of what we are doing
                enter_HCR();
                printf("Ph %d is %s\n", ID, (state == Thinking) ? "Thinking" : "Sleeping");
                exit_HCR();

                // And transit to the next state after "Thinking" or "Sleeping".
                state = (state == Thinking) ? Eating : Thinking;
                // An improvement could be doing this action for a
                // random amount of time.
                __delay32(396288UL);
                break;
            }
        }
    }
}

void philosopher_hierarchy() {
    // Save and store who we are
    const int ID = give_me_my_id();

    // The philospher always starts thinking.
    // An improvement consist in starting at a random inital state.
    ph_state state = Thinking;
    while (true) {
        switch (state) {
            case Eating:
            {
                // The philosopher takes the fork to his left
                int fork1 = mod(ID - 1, 5);
                // and the fork to his right
                int fork2 = ID;
                // Create a temp variable for storing fork value
                int tmp = fork1;

                // Always store in 'fork1' the maximum priority fork
                fork1 = max(fork1, fork2);
                // and in 'fork2'  the minimum priority fork
                fork2 = min(tmp, fork2);
                
                bool eating = false;

                // Enter critial region to check if we own one
                // of the two required forks
                enter_HCR();

                // If "forks[idx] == ID" then the philospher owns it.
                // If equals -1 it's owned by none.
                if ((forks[fork1] == ID) || (forks[fork1] == -1)) {
                    // Take the maximum priority fork and keep it
                    forks[fork1] = ID;

                    // If the second required fork is available then also take it
                    if (forks[fork2] == -1) {
                        forks[fork2] = ID;
                        eating = true;
                    }
                    // This way, we ensure that all philosophers always takes
                    // the maximum priority fork first and then the minimum priority one.
                    // For most of them, the first taken fork is the right one
                    // and the second is the left one, except for the
                    // philospher 0, which first takes the left fork
                    // and then the right one.
                }
                exit_HCR();

                // Simulation of the philosopher eating
                if (eating) {
                    enter_HCR();
                    printf("Ph %d is eating\n", ID);
                    exit_HCR();

                    // The eating process - an improvement could be
                    // setting a random delay time
                    __delay32(396288UL);

                    enter_HCR();
                    // Finally, leave both forks and transit to next state (sleep)
                    forks[fork1] = -1;
                    forks[fork2] = -1;
                    exit_HCR();

                    state = Sleeping;
                }
                break;
            }
                // Actually, 'Thinking' and 'Sleeping' do the same:
                // print a message and delay for an arbitrary time,
                // so we can handle both of them in the same case definition
            case Thinking:
            case Sleeping:
            {
                // Inform of what we are doing
                enter_HCR();
                printf("Ph %d is %s\n", ID, (state == Thinking) ? "Thinking" : "Sleeping");
                exit_HCR();

                // And transit to the next state after "Thinking" or "Sleeping"
                state = (state == Thinking) ? Eating : Thinking;

                // An improvement could be doing this action for a
                // random amount of time
                __delay32(396288UL);
                break;
            }

        }
    }
}

int main(void) {
    init_micro();
    init_ports();
    init_uart();

    SRbits.IPL = 0;


    init_task_manager();

    for (int i = 0; i < N_PH; i++) {
        // Define this macro for using the hierarchy algorithm
#ifdef HIERARCHY_ALG
        create_task((unsigned int) &philosopher_hierarchy, 200);
#else
        create_task((unsigned int) &philosopher, 200);
#endif
    }

    Idle_process();

    Sleep();
    return (0);
}
