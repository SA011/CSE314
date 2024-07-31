#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <wait.h>
#include <pthread.h>


void print_cooking(int chef) {
  printf("Chef %d is cooking\n\n", chef);
}

void print_pusher(int ingred) {
  printf("Agent provided ingredient no %d\n", ingred);
}
//condition variables
pthread_cond_t* cooker;
pthread_cond_t* pusher;
pthread_cond_t agent;
/////////////////////
//Mutex
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
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
    pthread_mutex_lock(&lock);                        //acquire lock
    while(cook != cooker_id && items_to_be_cooked > 0) //or cooking er time ekhono ashe nai and ekhono kisu cook howa baki
      pthread_cond_wait(&cooker[cooker_id], &lock);   //wait for agent

    if(items_to_be_cooked <= 0)break;                 //nothing to cook
    
    print_cooking(cooker_id);                         //cook korlam
    items_to_be_cooked--;
    ingredient = 0;                             //all ingredient used hoye gelo cooking e
    cook = -1;                                  //currently r keu cook nai tai -1
    
    pthread_cond_signal(&agent);                //signal agent
    pthread_mutex_unlock(&lock);                //Cooked. Realease lock
  }
  
  pthread_mutex_unlock(&lock);                  //broke with lock
  return 0;
}

//pusher will push a certain ingredient
void *pusher_loop(void *data)
{
  int pusher_id = *((int *)data);

  while(1)
  {
    pthread_mutex_lock(&lock);                        //acquire lock
    while((ingredient_requested >> pusher_id & 1) == 0 && items_to_be_cooked > 0) //cook ekhono kisu kora baki and agent jei ingredient request korse sheita o na
      pthread_cond_wait(&pusher[pusher_id], &lock);   //wait for agent

    if(items_to_be_cooked <= 0)break;           //nothing to push
    
    ingredient |= (1 << pusher_id);             //add ingredient korlam oi bit ta 1 kore
    ingredient_requested ^= (1 << pusher_id);   //request fulfilled tai oi bit ta flip kore 0 kore dilam
    print_pusher(pusher_id);
    if(__builtin_popcount(ingredient) == number_of_ingredient - 1){   //jodi ekta bade baki shob ingredient peye jai taile cooker ke signal dibo
      //wake up cooker
      cook = __builtin_ctz(ingredient^(-1));    //kon ingredient ta nai sheta ber korlam
      pthread_cond_signal(&cooker[cook]);       //wake up cooker with id cook  
    }

    pthread_mutex_unlock(&lock);                //Cooked. Realease lock
  }
  
  pthread_mutex_unlock(&lock);                  //broke with lock
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
  cooker = (pthread_cond_t*)malloc(sizeof(pthread_cond_t) * number_of_ingredient);  
  pusher = (pthread_cond_t*)malloc(sizeof(pthread_cond_t) * number_of_ingredient);
  
  cooker_thread = (pthread_t *)malloc(sizeof(pthread_t) * number_of_ingredient);
  pusher_thread = (pthread_t *)malloc(sizeof(pthread_t) * number_of_ingredient);

  
  cooker_thread_id = (int *)malloc(sizeof(int) * number_of_ingredient);
  pusher_thread_id = (int *)malloc(sizeof(int) * number_of_ingredient);

  for (int i = 0; i < number_of_ingredient; i++){
    cooker_thread_id[i] = i;
    pthread_cond_init(&cooker[i], NULL);
    pthread_create(&cooker_thread[i], NULL, cooker_loop, (void *)&cooker_thread_id[i]);
  }
  
  for (int i = 0; i < number_of_ingredient; i++){
    pusher_thread_id[i] = i;
    pthread_cond_init(&pusher[i], NULL);
    pthread_create(&pusher_thread[i], NULL, pusher_loop, (void *)&pusher_thread_id[i]);
  }
  
  pthread_cond_init(&agent, NULL);

  while(items_to_be_cooked > 0){
    // printf("WHAT %d\n", items_to_be_cooked);
    int missing = getRandom(0, number_of_ingredient - 1);   //ekta random ingredient nilam jeta bade baki gula agent dibe
    // printf("missing %d\n", missing);
    for(int i = 0; i < number_of_ingredient; i++){
      if(i != missing){       //missing chara baki pusher gula ke signal korbo 
        pthread_mutex_lock(&lock);      //look nisi
        ingredient_requested |= (1 << i);     //request kortesi sheta bolsi
        pthread_cond_signal(&pusher[i]);      //oi pusher ke signal korsi
        pthread_mutex_unlock(&lock);          //look charsi
      }
    }
    pthread_mutex_lock(&lock);                //lock nisi
    while(ingredient_requested != 0 || ingredient != 0){    //ingredient request 0 hoye gele bujhbo shob pusher nijer kaj korse
                                                            //ingredient 0 howa mane shob cooker o nijer kaj kore felse
      pthread_cond_wait(&agent, &lock);                     //duitai jotokhon 0 na hoy wait korbo
    }
    pthread_mutex_unlock(&lock);                      //lock chere diye aro ingredient dite loop back korbo
  }
  // printf("Kaj shesh\n");
  //wait for all threads to complete
  for (int i = 0; i < number_of_ingredient; i++)
  {
    pthread_cond_signal(&cooker[i]);                
    pthread_join(cooker_thread[i], NULL);
    printf("cooker %d joined\n", i);
  }
  
  
  //wait for all threads to complete
  for (int i = 0; i < number_of_ingredient; i++)
  {
    pthread_cond_signal(&pusher[i]);
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
