#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <math.h>
#include <time.h>

int *buffer;

struct timeval t_a, t_b, t_c;
double ta;
double tb;
double tc;

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
  int prod_id = (int)(intptr_t)param;
    

  for (i = prod_id; i < N; i+=P) 
  {
    if (i%P == prod_id)
    {
      sem_wait(&emptyBuf);
      pthread_mutex_lock(&mutex);
      buffer[tail] = i;
      tail = (tail+1) % B;
      //printf("Produced: %d\tpid = %d\n", i, prod_id);
      fflush(stdout);
      pthread_mutex_unlock(&mutex);
      sem_post(&fullBuf);
    }
  }
}

void *consumer(void *param)
{
  const int B_local = B;
  int count = 0;
  int sq_root = 0;
  int receivedInt = 0;

  int con_id = (int)(intptr_t)param;

  if (special_flag == 1)
  {
    if (consumer_array[C] == con_id)
    {
      sem_wait(&fullBuf);
      pthread_mutex_lock(&mutex);
      receivedInt = buffer[head];
      head = (head + 1) % B_local;
      //printf("Consumed: %d\tcid = %d\n", receivedInt, con_id);
      
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
    head = (head + 1) % B_local;
    //printf("Consumed: %d\tcid = %d\n", receivedInt, con_id);
    
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
  
  //printf("Threads\n");
  //printf("N = %d\tB = %d\tP = %d\tC = %d\n",N,B,P,C);
  if (N % C != 0)
  {
    special_flag = 1;
  }
  
  buffer = (int *)malloc(sizeof(int)*B);
  consumer_array = (int *)malloc(sizeof(int)*C);     

  pthread_mutex_init(&mutex, NULL);
  sem_init(&emptyBuf, 0, B);
  sem_init(&fullBuf, 0, 0);

  pthread_t prod_thread[P];
  pthread_t con_thread[C];
  
  int i;
  int k;
  
  gettimeofday(&t_a, NULL);
  ta = t_a.tv_sec + t_a.tv_usec/1000000.0;
  //Spawn Produer Threads
  for(i = 0; i < P; i++)
  {     
      if (pthread_create(&prod_thread[i],NULL,producer,(void *)(intptr_t)i) != 0)
      {
	  printf ("Unable to create producer thread\n");
	  exit(1);
      }
  }
  
  //Spawn Consumer Threads
  for(k = 0; k < C; k++)
  {
      consumer_array[k] = k;
      
      if (pthread_create(&con_thread[k],NULL,consumer, (void *)(intptr_t)k) != 0)
      {
	  printf ("Unable to create consumer thread\n");
	  exit(1);
      }
  }
  
  gettimeofday(&t_b, NULL);
  tb = t_b.tv_sec + t_b.tv_usec/1000000.0;
    
  //Join Terminated Producer Threads
  for (i = 0; i < P; i++)
  {
    pthread_join(prod_thread[i],NULL);
  }
 
  //Join Terminated Consumer Threads
  for (k = 0; k < C; k++)
  {
    pthread_join(con_thread[k],NULL);
  }
  
  printf("...\n");
  printf("System Execution Time: %.6lf seconds\n", tb-ta);
  return 0;
  
  return 0;
}
