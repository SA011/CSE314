#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include <unistd.h>
#include "zemaphore.h"

#define NUM_THREADS 26
//#define NUM_ITER 2

struct zemaphore zema[NUM_THREADS];

void *justprint(void *data)
{
  int thread_id = *((int *)data);

  for(int i=0; ; i++)
    {
      zem_down(&zema[thread_id]);
      printf("%c\n", (char)('z' - thread_id));
      //usleep(200000);
      zem_up(&zema[(thread_id + 1) % NUM_THREADS]);
    }
  return 0;
}

int main(int argc, char *argv[])
{

  pthread_t mythreads[NUM_THREADS];
  int mythread_id[NUM_THREADS];

  
  for(int i =0; i < NUM_THREADS; i++)
    {
      zem_init(&zema[i], (i == 0));
      mythread_id[i] = i;
      pthread_create(&mythreads[i], NULL, justprint, (void *)&mythread_id[i]);
    }
  
  for(int i =0; i < NUM_THREADS; i++)
    {
      pthread_join(mythreads[i], NULL);
    }
  
  return 0;
}
