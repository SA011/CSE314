#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>

int item_to_produce, curr_buf_size;
int total_items, max_buf_size, num_workers, num_masters;

int *buffer;

void print_produced(int num, int master) {

  printf("Produced %d by master %d\n", num, master);
}

void print_consumed(int num, int worker) {

  printf("Consumed %d by worker %d\n", num, worker);
}

//condition variables
pthread_cond_t buffer_full = PTHREAD_COND_INITIALIZER;
pthread_cond_t buffer_empty = PTHREAD_COND_INITIALIZER;
/////////////////////
//Mutex
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
///////


//produce items and place in buffer
//modify code below to synchronize correctly
void *generate_requests_loop(void *data)
{
  int thread_id = *((int *)data);

  while(1)
  {
    pthread_mutex_lock(&lock);                  //acquire lock
    while(curr_buf_size == max_buf_size)        //buffer full
      pthread_cond_wait(&buffer_full, &lock);   //wait for consumer to consume

    if(item_to_produce >= total_items) {
      break;                                    //all item produced. Break
    }
    
    buffer[curr_buf_size++] = item_to_produce;  //new item buffer e rakhlam
    print_produced(item_to_produce, thread_id); 
    item_to_produce++;
    
    pthread_cond_signal(&buffer_empty);         //signal consumer
    pthread_mutex_unlock(&lock);                //Produced. Realease lock
  }
  
  // printf("PRODUCE DONE %d\n", thread_id);
  // fflush(stdout);
  pthread_cond_broadcast(&buffer_empty);        //producer break kora mane shob production shesh
                                                //so ekhon shob consumer kaj shesh kore terminate korbe
                                                //tai shobaike uthaite broadcast korsi

  pthread_mutex_unlock(&lock);                  //break korar time e lock niye break korse
  return 0;
}

//write function to be run by worker threads
//ensure that the workers call the function print_consumed when they consume an item
void *generate_responses_loop(void *data)
{
  int thread_id = *((int *)data);

  while(1)
  {
    pthread_mutex_lock(&lock);                    //acquire lock
    while(curr_buf_size == 0 && item_to_produce < total_items)      //buffer khali hobe and ekhono notun kisu ashar chance ase
      pthread_cond_wait(&buffer_empty, &lock);   //wait for consumer to consume

    if(item_to_produce >= total_items && curr_buf_size == 0) {
      break;                                      //produce o shesh buffer o khali
    }
    
    int item_to_consume = buffer[--curr_buf_size];  //ekta item consumed
    print_consumed(item_to_consume, thread_id);
    
    pthread_cond_signal(&buffer_full);          //signal producer
    pthread_mutex_unlock(&lock);                //Consumed. Realease lock
  }
  // printf("CONSUME DONE %d\n", thread_id);
  // fflush(stdout);
  pthread_cond_broadcast(&buffer_full);         //same as producer. Consume shesh shob
                                                //so kono producer sleep e thakle uthaite hobe
                                                //tai broadcast korlam

  pthread_mutex_unlock(&lock);                  //broke with lock
  return 0;
}



int main(int argc, char *argv[])
{
  int *master_thread_id;
  pthread_t *master_thread;
  item_to_produce = 0;
  curr_buf_size = 0;
  
  int i;
  
   if (argc < 5) {
    printf("./master-worker #total_items #max_buf_size #num_workers #masters e.g. ./exe 10000 1000 4 3\n");
    exit(1);
  }
  else {
    num_masters = atoi(argv[4]);
    num_workers = atoi(argv[3]);
    total_items = atoi(argv[1]);
    max_buf_size = atoi(argv[2]);
  }
    

   buffer = (int *)malloc (sizeof(int) * max_buf_size);

   //create master producer threads
   master_thread_id = (int *)malloc(sizeof(int) * num_masters);
   master_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_masters);
  for (i = 0; i < num_masters; i++)
    master_thread_id[i] = i;

  for (i = 0; i < num_masters; i++)
    pthread_create(&master_thread[i], NULL, generate_requests_loop, (void *)&master_thread_id[i]);
  
  //create worker consumer threads
  int* worker_thread_id = (int *)malloc(sizeof(int) * num_workers);
  pthread_t* worker_thread = (pthread_t *)malloc(sizeof(pthread_t) * num_workers);
  
  for (i = 0; i < num_workers; i++)
    worker_thread_id[i] = i;

  for (i = 0; i < num_workers; i++)
    pthread_create(&worker_thread[i], NULL, generate_responses_loop, (void *)&worker_thread_id[i]);
  
  
  //wait for all threads to complete
  for (i = 0; i < num_masters; i++)
  {
    pthread_join(master_thread[i], NULL);
    printf("master %d joined\n", i);
  }
  
  
  //wait for all worker threads to complete
  for (i = 0; i < num_workers; i++)
  {
    pthread_join(worker_thread[i], NULL);
    printf("worker %d joined\n", i);
  }

  /*----Deallocating Buffers---------------------*/
  free(buffer);
  free(master_thread_id);
  free(master_thread);
  
  return 0;
}
