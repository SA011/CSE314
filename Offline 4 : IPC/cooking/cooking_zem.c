#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>
#include "zemaphore.h"


void print_cooking(int chef) {
  printf("Chef %d is cooking\n\n", chef);
}

void print_pusher(int ingred) {
  printf("Agent provided ingredient no %d\n", ingred);
}
//condition variables
struct zemaphore* cooker;
struct zemaphore* pusher;
struct zemaphore agent;
/////////////////////
//Mutex
struct zemaphore lock = PTHREAD_MUTEX_INITIALIZER;
///////
int cook = -1;
int ingredient = 0;
int ingredient_requested = 0;
int items_to_be_cooked;
int number_of_ingredient;

//cook items
void *cooker_loop(void *data)
{
  int cooker_id = *((int *)data);

  while(1)
  {
    zem_down(&cooker[cooker_id]);

    zem_down(&lock);                                  //acquire lock
    if(items_to_be_cooked <= 0)break;                 //nothing to cook
    
    print_cooking(cooker_id);
    items_to_be_cooked--;
    ingredient = 0;                             //all ingredient used
    cook = -1;
    zem_up(&agent);                //signal agent
    zem_up(&lock);                //Cooked. Realease lock
  }
  
  zem_up(&lock);                 //broke with lock
  return 0;
}

//pusher will push a certain ingredient
void *pusher_loop(void *data)
{
  int pusher_id = *((int *)data);

  while(1)
  {
    zem_down(&pusher[pusher_id]);   //wait for agent
    
    zem_down(&lock);                        //acquire lock
    if(items_to_be_cooked <= 0)break;                 //nothing to push
    
    ingredient |= (1 << pusher_id);             //add ingredient
    print_pusher(pusher_id);
    if(__builtin_popcount(ingredient) == number_of_ingredient - 1){
      //wake up cooker
      cook = __builtin_ctz(ingredient^(-1));
      zem_up(&cooker[cook]);       //wake up cooker with id cook  
    }

    zem_up(&lock);                //Cooked. Realease lock
  }
  
  zem_up(&lock);                  //broke with lock
  return 0;
}


int getRandom(int l, int r){
  return rand() % (r - l + 1) + l;
}
int main(int argc, char *argv[])
{
  pthread_t *cooker_thread;
  pthread_t *pusher_thread;
  int *cooker_thread_id;
  int *pusher_thread_id;
  items_to_be_cooked = 0;
  number_of_ingredient = 0;
  srand(clock());
   if (argc < 3) {
    printf("./cooking_cv #total_items_to_be_cooked #number_of_ingredient e.g. ./cooking_cv 10000 3\n");
    exit(1);
  }
  else {
    items_to_be_cooked = atoi(argv[1]);
    number_of_ingredient = atoi(argv[2]);
  }

  if(number_of_ingredient > 30 || number_of_ingredient < 2){
    printf("Maximum number of ingredient is 30 and Minimum number of ingredient is 2\n");
    exit(1);
  }
  cooker = (struct zemaphore*)malloc(sizeof(struct zemaphore) * number_of_ingredient);  
  pusher = (struct zemaphore*)malloc(sizeof(struct zemaphore) * number_of_ingredient);
  
  cooker_thread = (pthread_t *)malloc(sizeof(pthread_t) * number_of_ingredient);
  pusher_thread = (pthread_t *)malloc(sizeof(pthread_t) * number_of_ingredient);

  
  cooker_thread_id = (int *)malloc(sizeof(int) * number_of_ingredient);
  pusher_thread_id = (int *)malloc(sizeof(int) * number_of_ingredient);

  for (int i = 0; i < number_of_ingredient; i++){
    cooker_thread_id[i] = i;
    zem_init(&cooker[i], 0);
    pthread_create(&cooker_thread[i], NULL, cooker_loop, (void *)&cooker_thread_id[i]);
  }
  
  for (int i = 0; i < number_of_ingredient; i++){
    pusher_thread_id[i] = i;
    zem_init(&pusher[i], 0);
    pthread_create(&pusher_thread[i], NULL, pusher_loop, (void *)&pusher_thread_id[i]);
  }
  
  zem_init(&agent, 0);
  zem_init(&lock, 1);
  while(items_to_be_cooked > 0){
    int missing = getRandom(0, number_of_ingredient - 1);
    for(int i = 0; i < number_of_ingredient; i++){
      if(i != missing){
        zem_up(&pusher[i]);
      }
    }
    zem_down(&agent);
  }
  // printf("Kaj shesh\n");
  //wait for all threads to complete
  for (int i = 0; i < number_of_ingredient; i++)
  {
    zem_up(&cooker[i]);
    pthread_join(cooker_thread[i], NULL);
    printf("cooker %d joined\n", i);
  }
  
  
  //wait for all threads to complete
  for (int i = 0; i < number_of_ingredient; i++)
  {
    zem_up(&pusher[i]);
    pthread_join(pusher_thread[i], NULL);
    printf("pusher %d joined\n", i);
  }
  
  
  /*----Deallocating Buffers---------------------*/
  free(cooker);
  free(cooker_thread);
  free(cooker_thread_id);
  
  free(pusher);
  free(pusher_thread);
  free(pusher_thread_id);
  return 0;
}
