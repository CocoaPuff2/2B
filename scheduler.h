#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <queue>
#define DEFAULT_QUANTUM 1000000 // 1 sec

using namespace std;

class Scheduler {
public:
  Scheduler( ) { };
  Scheduler( int quantum );
  bool addProcess( int pid ); // push pid into the queue at level 0.
  void run_rr( );             // run processes as a round-robin scheduler
  void run_mfq( );            // run processes as a multi-level feedback queue scheduler
private:
  int msec = DEFAULT_QUANTUM; // a default time quantum
  int nQuantums = 0;          // # quantums spent so far
  std::queue<int> queue[3];   // a multi-level queue
  void schedulerSleep( );     // let the scheduler wait for a given quatumn to run the current process
};

#endif // SCHEDULER_H
