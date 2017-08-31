#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>

typedef int buffer_item;
#define BUFFER_SIZE 10
#define RAND_DIVISOR 100000000
#define TRUE 1

pthread_mutex_t mutex;
pthread_mutex_t mutex2;
pthread_mutex_t mutex3;
sem_t full;
sem_t empty;
buffer_item buffer[BUFFER_SIZE];

int counter;
int check = 1;
int check1 = 1;
int first = 1;
int second = 0;
buffer_item item;
void srand(unsigned int seed);

pthread_t tid;       //Thread ID
pthread_attr_t attr; //Set of thread attributes

void *producer(void *param);
void *consumer(void *param);

void initializeData() 
{
   pthread_mutex_init(&mutex, NULL);
   pthread_mutex_init(&mutex2, NULL);
   pthread_mutex_init(&mutex3, NULL);
   sem_init(&full, 0, 0);
   sem_init(&empty, 0, BUFFER_SIZE);
   pthread_attr_init(&attr);
   counter = 0;
}

int insert_item(buffer_item item)
{
   if(counter < BUFFER_SIZE) 
   {
      buffer[counter] = item;
      counter++;
      return 0;
   }
   else 
   {
      return -1;
   }
}

int remove_item(buffer_item *item) 
{
   if(counter > 0)
   {
      *item = buffer[(counter-1)];
      counter--;
      return 0;
   }
   else 
   { 
      return -1;
   }
}

void *producer(void *param)
{
	srand(time(NULL));
   while(TRUE) 
   {
      int rNum = rand() / RAND_DIVISOR;
      sleep(rNum);
      
      while(TRUE)
      {
	pthread_mutex_lock(&mutex2);
	if(check==0)
	{
	  pthread_mutex_unlock(&mutex2);
	  break;
	}        
	item = (rand()%100)+1;
        check1 =0;
        pthread_mutex_unlock(&mutex2);
	while(check1==0);
      }
     
      check=1;
      sem_wait(&empty);
      pthread_mutex_lock(&mutex);
      if(insert_item(item)) 
      {
         printf("report error condition\n");
      }
      else
      {
         printf("producer produced %d\n", item);
      }
      pthread_mutex_unlock(&mutex);
      sem_post(&full);
      
   }
}

void *producermonitor(void *param) 
{
   
   while(TRUE)
   {
	while(check1);
	pthread_mutex_lock(&mutex2);   
	if(item > 50)
	{
	  printf("-------reject produce item : %d--------\n",item);
	  check1 =1;
	  pthread_mutex_unlock(&mutex2);
	}
	else
	{
	  check=0;
	  check1 =1;
	  pthread_mutex_unlock(&mutex2);
	}  
   }
   
}

void *consumer(void *param) 
{
   while(TRUE) 
   {    
      int rNum = rand() / RAND_DIVISOR;
      sleep(rNum);
      sem_wait(&full);
      pthread_mutex_lock(&mutex);
      while(TRUE)
      {
	if(second==1)
	  break;
	
	first = 0;
	while(first==0);
	pthread_mutex_lock(&mutex3);
      }

      second = 0;
      
      
      if(remove_item(&item))
      {
         printf("Consumer report error condition\n");
      }
      else 
      {
         printf("consumer consumed %d\n", item);
      }
      
      pthread_mutex_unlock(&mutex);
      sem_post(&empty);
      pthread_mutex_unlock(&mutex3);
    }
}


void *consumermonitor(void *param) 
{  
   while(TRUE) 
	{ 
	  while(first);
	  pthread_mutex_lock(&mutex3); 
	  int con_item = buffer[(counter-1)];
	  if(con_item > 25)
	  {
	    printf("--------reject consume item : %d ---------\n",con_item);
	    con_item = con_item/2;
	    buffer[(counter-1)] = con_item;
	    first=1;
	    second =1;
	    pthread_mutex_unlock(&mutex3);
	  }
	  else
	  {
	    first = 1;
	    second =1;
	    pthread_mutex_unlock(&mutex3);
	  }
    	}
 
}

int main(int argc, char *argv[])
 {
   int i;

   if(argc != 4) 
   {
      fprintf(stderr, "USAGE:./main.out <INT> <INT> <INT>\n");
   }

   int mainSleepTime = atoi(argv[1]);
   int numProd = atoi(argv[2]); 
   int numCons = atoi(argv[3]); 

   initializeData();
   pthread_create(&tid,&attr,producermonitor,NULL);
   pthread_create(&tid,&attr,consumermonitor,NULL);
   for(i = 0; i < numProd; i++)
   {
      pthread_create(&tid,&attr,producer,NULL);
   }

   for(i = 0; i < numCons; i++) 
   {
        pthread_create(&tid,&attr,consumer,NULL);
   }
   
   sleep(mainSleepTime);

   printf("Exit the program\n");
   exit(0);
}
