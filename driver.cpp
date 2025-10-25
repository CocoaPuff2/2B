#include <iostream>    // cout, cerr
#include <unistd.h>    // fork
#include <sys/wait.h>  // wait
#include "scheduler.h"

using namespace std;

int session = 0;

void *scheduler_thread( void* scheduler ) {
  if ( session == 0 )
    ( (Scheduler *)scheduler )->run_rr( );
  else // session == 1
    ( (Scheduler *)scheduler )->run_mfq( );    

  return NULL;
}

int main( int argc, char* argv[] ) {
  // validate arguments
  if ( argc < 3 || argc % 2 == 0 ) {
    cerr << "usage: driver taskA secs taskB secs ..." << endl;
    exit( -1 );
  }

  int nTasks = ( argc - 1 ) / 2;
  char* names[nTasks];
  char* secs[nTasks];

  for ( int i = 0; i < nTasks; i++ ) {
    names[i] = argv[i * 2 + 1];
    secs[i] = argv[i * 2 + 2];
    cout << "driver: task[" << names[i] << "] runs for " << secs[i] << "seconds." << endl;
  }



  // two sessions: (1) round robin and (2) multilevel feedback queue
  for ( session = 0; session < 2; session++ ) {
    int timeQuantum = ( session == 0 ) ? 2000000 : 1000000; // rr uses 2000 msec, whereas mfq uses 1000msec
    Scheduler scheduler( timeQuantum ); // 2 sec in rr and 1 sec in mfq
    
    // submit tasks
    cout << "session " << session << ":"
	 << ( (session == 0) ? "round robin" : "multilevel feedback queue" )
	 << " scheduler ----------------------------------------------------------" << endl;
    cout << "parent = " << getpid( ) << endl;
    
    int pid; // a child process id
    for ( int i = 0; i < nTasks; i++ ) {
      if ( ( pid = fork( ) ) > 0 ) { // parent
	scheduler.addProcess( pid );
      }
      else {                         // child
	cout << "driver: child = " << getpid( ) << endl;
	execlp( "./task", "task", names[i], secs[i], (char *)NULL );
      }
    }

    // run scheduler
    pthread_t tid;
    pthread_create( &tid, NULL, scheduler_thread, (void *)&scheduler );
    
    // wait for child terminations
    for ( int i = 0; i < nTasks; i++ )
      wait( NULL );
    
    // done
    cout << "driver: All tasks are done" << endl;
    pthread_join( tid, NULL );
    cout << "driver: confirms the scheduler thread termination" << endl;
  }
}
