#include <stdio.h>       /* for fprintf() */
#include <stdlib.h>      /* for exit() */
#include <unistd.h>      /* for sleep() */
#include <pthread.h>     /* for pthreads functions */

#define NTHREADS 2

int string_index = 0;

char string_to_print[] = "0123456789";

pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void *A(void *arg)
{
   int tmp_index;


   sleep(1);   /* sleep 1 sec to allow all threads to start */

   while (1) {
      pthread_mutex_lock(&lock);
      tmp_index = string_index;

      if (tmp_index % 2 == 1){
	pthread_cond_wait(&cond, &lock);
	tmp_index = string_index;
      }
	if (!(tmp_index+2 > sizeof(string_to_print))) {
          printf("A%c ", string_to_print[tmp_index]);
          usleep(1);   /* sleep 1 usec */
        }
 
      string_index = tmp_index + 1;
      pthread_cond_signal(&cond);

      if (tmp_index+2 > sizeof(string_to_print)) {
         printf("\n");
         string_index = 0;   /* wrap around */
      }
      pthread_mutex_unlock(&lock);
   }
}

void *B(void *arg)
{
   int tmp_index;

   sleep(1);   /* sleep 1 sec to allow all threads to start */



   while (1) {
      pthread_mutex_lock(&lock);
      tmp_index = string_index;

      if (tmp_index %2 == 0) {
	pthread_cond_wait(&cond, &lock);
	tmp_index = string_index;
      }

      if (!(tmp_index+2 > sizeof(string_to_print))) {
         printf("B%c ", string_to_print[tmp_index]);
         usleep(1);   /* sleep 1 usec */
      }


      string_index = tmp_index + 1;
      pthread_cond_signal(&cond);

      if (tmp_index+2 > sizeof(string_to_print)) {
         printf("\n");
         string_index = 0;   /* wrap around */
      }
      pthread_mutex_unlock(&lock);
   }
}


int main(void)
{
   pthread_t thread_A, thread_B;
   int k;

   printf("Starting Thread A");
   if (pthread_create(&thread_A, NULL, &A, NULL) != 0) {
       printf("Error creating thread A");
       exit(-1);
   }
   printf("Starting Thread B");
   if (pthread_create(&thread_B, NULL, &B, NULL) != 0) {
       printf("Error creating thread A");
       exit(-1);
   }

   sleep(20);   /* sleep 20 secs to allow time for the threads to run */
                /* before terminating them with pthread_cancel()      */


   pthread_cancel(thread_A);
   pthread_cancel(thread_B);


   pthread_exit(NULL);
}
