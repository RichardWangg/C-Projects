#define _XOPEN_SOURCE 500 

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <openssl/sha.h>
#include "dontmodify.h"

#define NUM_ELVES 5

/* Global Variables -- Add more if you need! */
int total_tasks;
int active_tasks;
pthread_mutex_t mutex;
sem_t empty_list;
sem_t full_list;

/* Function Prototypes for pthreads */
void* dobby( void* );
void* house_elf( void * );

/* Don't change the function prototypes below;
   they are your API for how the house elves do work */

/* Removes a task from the list of things to do
   To be run by: house elf threads
   Takes no arguments
   NOT thread safe!
   return: a pointer to a task for an elf to do
*/
task* take_task();

/* Perform the provided task
   To be run by: house elf threads
   argument: pointer to task to be done
   IS thread safe
   return: nothing
*/  
void do_work( task* todo );

/* Put tasks in the list for elves to do
   To be run by: Dobby
   argument: how many tasks to put in the list
   NOT thread safe
   return: nothing
*/
void post_tasks( int howmany );

/* Used to unlock a mutex, if necessary, if a thread
   is cancelled when blocked on a semaphore
*/
void house_elf_cleanup( void * );



/* Complete the implementation of main() */

int main( int argc, char** argv ) {
  if ( argc != 2 ) {
      printf( "Usage: %s total_tasks\n", argv[0] );
      return -1;
  }
  /* Init global variables here */
  total_tasks = atoi( argv[1] );
  
  
  
  
  
  printf("There are %d tasks to do today.\n", total_tasks);
  
  /* Launch threads here */
  


  /* Wait for Dobby to be done */
  




  /* Cleanup Global Variables here */
  




  return 0;
}

/* Write the implementation of dobby() */

void* dobby( void * arg ) {






}  

/* Complete the implementation of house_elf() */

void* house_elf( void * ignore ) {
  /* The following function call registers the cleanup
     handler to make sure that pthread_mutex_t locks 
     are unlocked if the thread is cancelled. It must
     be bracketed with a call to pthread_cleanup_pop 
     so don't change this line or the other one */
  pthread_cleanup_push( house_elf_cleanup, NULL ); 
  while( 1 ) {


  }
  /* This cleans up the registration of the cleanup handler */
  pthread_cleanup_pop( 0 ) ;
}

/* Implement unlocking of any pthread_mutex_t mutex types
   that are locked in the house_elf thread, if any 
*/
void house_elf_cleanup( void* arg ) {




}
