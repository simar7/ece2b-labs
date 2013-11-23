#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>

int *buffer;

int N;
int B;
int P;
int C;

pthread_mutex_t mutex;
sem_t emptyBuf;
sem_t fullBuf;

int head = 0;
int tail = 0;

int special_flag = 0;
int* consumer_array;

void *producer(void *param)
{    
  int i = 0;
  int prod_id = (int)param;
    
  for (i = prod_id; i < N; i+=P) 
  {
    if (i%P == prod_id)
    {
      sem_wait(&emptyBuf);
      pthread_mutex_lock(&mutex);
      buffer[tail] = i;
      tail = (tail+1) % B;
      //printf("Produced: %d with pid = %d\n", i, prod_id);
      fflush(stdout);
      pthread_mutex_unlock(&mutex);
      sem_post(&fullBuf);
    }
  }
}

void *consumer(void *param)
{
  int count = 0;
  int sq_root = 0;
  int receivedInt = 0;

  int con_id = (int)param;

  if (special_flag == 1)
  {
    if (consumer_array[C] == con_id)
    {
      sem_wait(&fullBuf);
      pthread_mutex_lock(&mutex);
      receivedInt = buffer[head];
      head = (head + 1) % B;
      
      if (sqrt(receivedInt) == (int)sqrt(receivedInt))
      {
	printf("c%d %d %d\n", con_id, receivedInt,(int)sqrt(receivedInt));
      }
      
      fflush(stdout);
      pthread_mutex_unlock(&mutex);
      sem_post(&emptyBuf);
    }
  }
  
  while (count < N/C)
  {
    sem_wait(&fullBuf);
    pthread_mutex_lock(&mutex);
    receivedInt = buffer[head];
    head = (head + 1) % B;
    
    if (sqrt(receivedInt) == (int)sqrt(receivedInt))
    {
      printf("c%d %d %d\n", con_id, receivedInt,(int)sqrt(receivedInt));
    }
    
    fflush(stdout);
    pthread_mutex_unlock(&mutex);
    sem_post(&emptyBuf); 
    count++;
  }
}

int main(int argc, char *argv[])
{   
  N = atoi(argv[1]);
  B = atoi(argv[2]);
  P = atoi(argv[3]);
  C = atoi(argv[4]);
  
  printf("N = %d\tB = %d\tP = %d\tC = %d\n",N,B,P,C);
  if (N % C != 0)
  {
    special_flag = 1;
  }
  
  buffer = (int *)malloc(sizeof(int)*B);
  consumer_array = (int *)malloc(sizeof(int)*C);     

  pthread_mutex_init(&mutex, NULL);
  sem_init(&emptyBuf, 0, 10);
  sem_init(&fullBuf, 0, 0);

  pthread_t prod_thread[P];
  pthread_t con_thread[C];
  
  int i;
  
  //Spawn Produer Threads
  for(i = 0; i < P; i++)
  {      
      if (pthread_create(&prod_thread[i],NULL,producer,(void *)i) != 0)
      {
	  printf ("Unable to create producer thread\n");
	  exit(1);
      }
  }
  
  //Spawn Consumer Threads
  for(i = 0; i < C; i++)
  {
      consumer_array[i] = i;
      
      if (pthread_create(&con_thread[i],NULL,consumer,(void *)i) != 0)
      {
	  printf ("Unable to create consumer thread\n");
	  exit(1);
      }
  }
    
  //Join Terminated Producer Threads
  for (i = 0; i < P; i++)
  {
    pthread_join(prod_thread[i],NULL);
  }
 
  //Join Terminated Consumer Threads
  for (i = 0; i < C; i++)
  {
    pthread_join(con_thread[i],NULL);
  }
  
  return 0;
}