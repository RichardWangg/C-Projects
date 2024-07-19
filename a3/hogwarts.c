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
int remaining_tasks;
int total_completed;
pthread_mutex_t mutex;
pthread_t elf_threads[NUM_ELVES];
sem_t empty_list;
sem_t full_list;

/* Function Prototypes for pthreads */
void *dobby(void *);
void *house_elf(void *);

/* Don't change the function prototypes below;
   they are your API for how the house elves do work */

/* Removes a task from the list of things to do
   To be run by: house elf threads
   Takes no arguments
   NOT thread safe!
   return: a pointer to a task for an elf to do
*/
task *take_task();

/* Perform the provided task
   To be run by: house elf threads
   argument: pointer to task to be done
   IS thread safe
   return: nothing
*/
void do_work(task *todo);

/* Put tasks in the list for elves to do
   To be run by: Dobby
   argument: how many tasks to put in the list
   NOT thread safe
   return: nothing
*/
void post_tasks(int howmany);

/* Used to unlock a mutex, if necessary, if a thread
   is cancelled when blocked on a semaphore
*/
void house_elf_cleanup(void *);

/* Complete the implementation of main() */

int main(int argc, char **argv)
{
   if (argc != 2)
   {
      printf("Usage: %s total_tasks\n", argv[0]);
      return -1;
   }
   /* Init global variables here */
   total_tasks = atoi(argv[1]);
   printf("There are %d tasks to do today.\n", total_tasks);
   remaining_tasks = total_tasks;
   active_tasks = 0;
   total_completed = 0;
   pthread_t dobby_thread;
   pthread_mutex_init(&mutex, NULL);
   sem_init(&empty_list, 0, 0);
   sem_init(&full_list, 0, 0);

   /* Launch threads here */

   pthread_create(&dobby_thread, NULL, dobby, NULL);

   for (int i = 0; i < NUM_ELVES; i++)
   {
      pthread_create(&elf_threads[i], NULL, house_elf, NULL);
   }

   /* Wait for Dobby to be done */

   pthread_join(dobby_thread, NULL);

   /* Cleanup Global Variables here */

   printf("Total Tasks Completed: %d\n", total_completed);
   pthread_mutex_destroy(&mutex);
   sem_destroy(&empty_list);
   sem_destroy(&full_list);

   return 0;
}

/* Write the implementation of dobby() */

void *dobby(void *arg)
{
   while (1)
   {
      // Wait for empty list to be asserted
      sem_wait(&empty_list);
      printf("Dobby woke up\n");

      // Post 10 tasks unless < 10 are left
      pthread_mutex_lock(&mutex);
      if (remaining_tasks == 0 && active_tasks == 0)
      {
         printf("all tasks completed\n");

         // Dismiss all the slaves!
         for (int i = 0; i < NUM_ELVES; i++)
         {
            pthread_cancel(elf_threads[i]);
            pthread_join(elf_threads[i], NULL);
         }

         pthread_mutex_unlock(&mutex);
         break;
      }

      int tasks_to_post = remaining_tasks < 10 ? remaining_tasks : 10;
      remaining_tasks -= tasks_to_post;
      active_tasks += tasks_to_post;
      post_tasks(tasks_to_post);
      pthread_mutex_unlock(&mutex);

      // Wake up house elves
      for (int i = 0; i < tasks_to_post; i++)
      {
         sem_post(&full_list);
      }
   }
   return NULL;
}

/* Complete the implementation of house_elf() */

void *house_elf(void *ignore)
{
   /* The following function call registers the cleanup
      handler to make sure that pthread_mutex_t locks
      are unlocked if the thread is cancelled. It must
      be bracketed with a call to pthread_cleanup_pop
      so don't change this line or the other one */
   pthread_cleanup_push(house_elf_cleanup, NULL);
   while (1)
   {
      // Check if we need to wake up Dobby
      pthread_mutex_lock(&mutex);
      if (active_tasks == 0)
      {
         printf("waking up dobby...\n");
         sem_post(&empty_list);
      }
      pthread_mutex_unlock(&mutex);

      // Wait for a task
      sem_wait(&full_list);
      // pthread_testcancel();
      // Take a task
      pthread_mutex_lock(&mutex);
      task *todo = take_task();
      pthread_mutex_unlock(&mutex);

      // Do the work
      do_work(todo);
      active_tasks--;
      total_completed++;
      printf("acitve tasks: %d\n", active_tasks);
      // pthread_testcancel();
   }
   /* This cleans up the registration of the cleanup handler */
   pthread_cleanup_pop(0);
}

/* Implement unlocking of any pthread_mutex_t mutex types
   that are locked in the house_elf thread, if any
*/
void house_elf_cleanup(void *arg)
{
   if (pthread_mutex_trylock(&mutex) == 0)
   {
      pthread_mutex_unlock(&mutex);
   }
}
