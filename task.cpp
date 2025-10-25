#include <time.h>   // clock
#include <unistd.h> // getpid
#include <stdio.h>  // printf
#include <iostream> // cerr, endl, flush
#include <stdlib.h> // atoi
#include <math.h>   // pow

int main( int argc, char* argv[] ) {
  using namespace std;
  
  // validate arguments
  if ( argc != 3 ) {
    cerr << "argc = " << argc << ", usage: task name seconds" << endl;
    cerr << argv[0] << " " << argv[1] << endl;
    exit( -1 );
  }
  char* name = argv[1];
  int secs = atoi( argv[2] );
  cout << "task[" << name << "] with pid=" << getpid( ) << " runs for " << secs << " seconds" << endl;

  // timin variables
  clock_t org, start, end;
  double cpu_time_used;

  // dummy results
  double answer = 0;
  int count = 0;

  // set the original time
  org = clock();      

  for ( int time = 0; time < secs; time++ ) {
    cout << name << endl;

    // start clock
    start = clock();    
    do {
      // a dummy computation starts
      answer = 1.00000001;
      for ( int i = 0; i < 2000; i++ )
	answer = pow( answer, 1.01 );
      // a dummy computation ends

      // lap clock
      end = clock();
      cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
      
      count++;
    } while ( cpu_time_used < 1.0 );
    
  }
  // finish this task
  printf("\nTask[%s]: cpu time used: %lf seconds (answer = %f) count = %d\n", name, ((double)(end - org)) / CLOCKS_PER_SEC, answer, count);  
  
  return 0;
}
