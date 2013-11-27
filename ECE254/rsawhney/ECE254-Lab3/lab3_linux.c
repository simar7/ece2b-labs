#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>
#include <math.h>
#include <sys/types.h> 
#include <sys/wait.h>

struct timeval t_a, t_b, t_c;
double ta;
double tb;
double tc;

int child_status;

int id = 0;
int cid = 0;

int* consumer_array;
int special_flag = 0;

mqd_t qdes;
mqd_t ret_open;

char qname[] = "/mailbox_rsawhney";
mode_t mode = S_IRUSR | S_IWUSR;
struct mq_attr attr;

void produce (const int N, const int B, const int P, const int C, int id)
{
  qdes  = mq_open(qname, O_RDWR |O_CREAT, mode, &attr);

  if (qdes == -1) 
  {
    perror("[PRODUCER]\tmq_open() failed");
    exit(1);
  }
    
  int i;
  for (i = id; i < N; i+=P)
  {
    if (i%P == id)
    {
      if (mq_send(qdes, (char *)&i, sizeof(int), 0) == -1)
      {
	    perror("[PRODUCER]\tFAILED TO SEND TO QUEUE\n");
      }      
    }
  }
  
  if (mq_close(qdes) == -1) 
  {
    perror("[PRODUCER]\tmq_close() failed");
    exit(2);
  }   
}

void consume(const int N, const int B, const int P, const int C, int cid)
{
  if (B < N)
  {
    int received_int = 0;
    int count = 0;
    int sq_root = 0;
    
    int remainder = N % C;
    
    ret_open = mq_open(qname, O_RDONLY | O_CREAT, mode, &attr);
  
    if (special_flag == 1)
    {
        if (consumer_array[C] == cid)
        {
            if (mq_receive(ret_open, (char *)&received_int, sizeof(int), 0) == -1)
            {
                perror("[CONSUMER]\tFAILED TO RECEIVE FROM QUEUE\n");
            }
            if (sqrt(received_int) == (int)sqrt(received_int))
            {
                printf("c%d %d %d\n", cid, received_int,(int)sqrt(received_int));
                fflush(stdout);
            }
        }
    }

    while (count < N/C)
    { 
      if (mq_receive(ret_open, (char *)&received_int, sizeof(int), 0) == -1)
      {
	      perror("[CONSUMER]\tFAILED TO RECEIVE FROM QUEUE\n");
      }	        
      if (sqrt(received_int) == (int)sqrt(received_int))
      {
        printf("c%d %d %d\n", cid, received_int,(int)sqrt(received_int));
        fflush(stdout);
      }
      count++;
    }
  }
   
  if (mq_close(ret_open) == -1) 
  {
	  perror("[CONSUMER]\tmq_close() failed");
	  exit(2);
  }   
}

int main(int argc, char* argv[]) 
{
  int wait_status;

  const int N = atoi(argv[1]);
  const int B = atoi(argv[2]);
  const int P = atoi(argv[3]);
  const int C = atoi(argv[4]);
  
  consumer_array = (int *)malloc(sizeof(int)*C);
  
  if (N % C != 0)
  {
    special_flag = 1;
  }
  
  attr.mq_maxmsg  = B;
  attr.mq_msgsize = sizeof(int);
  attr.mq_flags   = 0;

  int p_plus_c = P + C;
  pid_t child_pid;
  int i;
  
  gettimeofday(&t_a, NULL);
  ta = t_a.tv_sec + t_a.tv_usec/1000000.0;
  
  for (i = 0; i < p_plus_c; i++)
  {
    child_pid = fork();
    
    if ((int)child_pid == 0)
    {
      if (i < P)
      {
	    produce(N,B,P,C,i);
      }
      else
      {
	    consumer_array[i-P] = i-P;
	    consume(N,B,P,C,i-P);
      }
      exit(1);
    }  
  }
  
  int k;
  for (k = 0; k < p_plus_c; k++)
  {
    wait (&child_status);
  }
  
  gettimeofday(&t_b, NULL);
  tb = t_b.tv_sec + t_b.tv_usec/1000000.0;
  
  if (mq_unlink(qname) != 0) 
  {
	  perror("mq_unlink() failed");
	  exit(3);
  }
  
  printf("...\n");
  fflush(stdout);
  printf("System Execution Time: %.6lf seconds\n", tb-ta);
  fflush(stdout);
  
  return 0;
}
