#include <signal.h>   // kill
#include <unistd.h>   // usleep
#include <iostream>   // cout, cerr, endl
#include "scheduler.h"

Scheduler::Scheduler( int quantum ) {
    msec = ( quantum > 0 ) ? quantum : DEFAULT_QUANTUM;
}

// puts a new child process PID into queue[0] aka highest priority.
// New tasks always start at the top.
bool Scheduler::addProcess( int pid ) {
    if ( kill( pid, SIGSTOP ) != 0 ) {
        cerr << "process[" << pid << "] can't be paused." << endl;
        return false;
    }
    queue[0].push( pid ); // enqueue this process
    return true;
}

void Scheduler::schedulerSleep( ) {
    usleep( msec );
    cerr << "scheduler: completed " << (++nQuantums) << " quantums" << endl;
}

// 1. repeatedly pop PID from front of queue
// 2. If process not alive, removes it
// 3. If alive, current, SIGCONT resumes process for next quantum w/ schedulerSleep,
//    then send kill(pid, SIGSTOP) to pause it.
// 4. nQuantums++, print status messages, and if a process finished while it ran,
//      detect and remove it. Cont until no more PIDS.

void Scheduler::run_rr( ) {
    cerr << "scheduler (round robin): started" << endl;
    int current = 0;

    while ( true ) {
        if ( queue[0].size( ) == 0 )         // no more processes to terminate scheduler
            break;
        current = queue[0].front( );         // pick up the first process from the queue

        if ( kill( current, 0 ) == 0 ) {      // current process is alive
            cerr << "\nscheduler: resumed " << current << endl;
            kill( current, SIGCONT );           // run it for a next quantum
            schedulerSleep( );
            kill( current, SIGSTOP );
        }

        queue[0].pop( );
        if ( kill( current, 0 ) == 0 ) {     // current process is still alive
            queue[0].push( current );
        }
        else { // current process is dead
            cerr << "scheduler: confirmed " << current << "'s termination" << endl;
            current = 0;
        }
    }
    cerr << "scheduler: has no more process to run" << endl;
}


// IMPLEMENT BELOW THIS LINE----------------------------------------------------------------------

// Manages 3 queues, new tasks go into queue[0]
// If process doesn't finish in 1 sec in queue[0], move to queue[1]. If q[0] empty, then run q[1]

// q[1]’s total quantum is 2 seconds, but after 1 second, if new tasks in queue[0], handle them first;
//  else  resume the same queue[1] process for 1 sec.

//  If after 2 seconds still isn’t done, move it to queue[2]
// ueue[2]’s total quantum is 4 seconds, but like queue[1],
// run it in 1-second slices and re-check higher queues between slices

void Scheduler::run_mfq( ) {
    cerr << "scheduler (multilevel feedback queue): started" << endl;
    int current = 0;  // current pid
    int previous = 0; // previous pid

    int slices[3];                          // slice[i] means that level i's current time slice.
    for ( int i = 0; i < 3; i++ )
        slices[i] = 0;                        // all levels start slice 0.

    while ( true ) {
        int level = 0;
        for ( ; level < 3; level++ ) {
            // if the current level's slice is 0.
            if (slices[level] == 0) {
                // check the current level queue is empty.
                if (queue[level].empty()) continue;  // if so, go to a next lower level queue.
                // otherwise, pick up a pid from this queue
                current = queue[level].front();
                queue[level].pop();
                break;

            }
            // if the current level's slide is 1, 2, or 3
            if (slices[level] > 0) {
                // The previous process should run continuously.
                current = previous;
                break;
            }

            if (slices[level] > 0) {
                // The previous process should run continuously.
                current = previous;
                break;
            }
        }
        // if we reached level 3, (i.e., the lowest level) and found no processes to schedule
        if (level == 3) {
            // finish scheduler.cpp
            cerr << "scheduler: has no more process to run" << endl;
            return;
        }

        // check if a process to run is still active.
        if (kill(current, 0) == 0) {
            // if so, resumt it, calls schedulerSleep( ) to give a time quantum.
            kill(current, SIGCONT);
            schedulerSleep();
            // then, suspends it.
            kill(current, SIGSTOP);
        }

        // check if this process is still active.
        if (kill(current, 0) == 0) {
            // if so and if the current level is 1 or 2, shift to a next slice
            if (level < 2) {
                slices[level]++;

                // if the next slice was wrapped back to 0. this pid should
                if (slices[level] >= (1 << level)) { // slices: 1 sec, 2, sec, 4 sec
                    slices[level] = 0;
                    queue[level + 1].push(current);

                    // or go back to the lowest level queue
                }
            }

        }

        // current process is dead, print out:
        cerr << "scheduler: confirmed " << current << "'s termination" << endl;
    }
    cerr << "scheduler: has no more process to run" << endl;
}

// IMPLEMENT ABOVE THIS LINE----------------------------------------------------------------------
