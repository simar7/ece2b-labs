#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <sys/stat.h>
#include <time.h>
#include <errno.h>
#include <sys/wait.h>

struct timeval t_a, t_b, t_c;
double ta;
double tb;
double tc;

void producer (int N, const int B)
{
    int randNum;
	int child_status;

    mqd_t qdes;
    char qname[] = "/mailbox_rsawhney";

	mode_t mode = S_IRUSR | S_IWUSR;

	struct mq_attr attr;

	attr.mq_maxmsg  = B;
	attr.mq_msgsize = sizeof(randNum);
	attr.mq_flags   = 0;	// blocking queue 

	//GET TIME A
	gettimeofday(&t_a, NULL);
	ta = t_a.tv_sec + t_a.tv_usec/1000000.0;

	pid_t child_pid;
	child_pid = fork();

    //printf("child_pid AFTER FORK(): %d\n", child_pid);

	if (child_pid != 0)
	{
		//In parent process (PRODUCER)
		//printf("PARENT Process ID: %d\n", (int)getpid());
		//printf("CHILD PROCESS ID within PARENT PROCESS: %d\n", (int)child_pid);
    
	    qdes  = mq_open(qname, O_RDWR |O_CREAT, mode, &attr);

        //printf("PRODUCER - mq_open result after creater and initializing: %d\n", qdes);

	    if (qdes == -1) 
	    {
		    perror("PRODUCER - mq_open() failed");
		    exit(1);
	    }
        
        int i = 0;
        
        gettimeofday(&t_b, NULL);
	    tb = t_b.tv_sec + t_b.tv_usec/1000000.0;
        printf("Initialization Time = %.6lf seconds\n", (tb - ta));

	    while (i < N)
	    {
		    randNum = (rand() % 9);
            //printf("randNum generated: %d\n", randNum);
            
            if (mq_send(qdes, (char *)&randNum, sizeof(randNum), 0) == -1)
            {
                perror("PRODUCER - FAILED TO SEND TO QUEUE");
            }

            printf("PRODUCER - Random Number %d added to queue\n", randNum);
            fflush(stdout);		    
            i++;
	    }

	    // GET TIME C
        gettimeofday(&t_c, NULL);
	    tc = t_c.tv_sec + t_c.tv_usec/1000000.0;
        printf("Transmission Time = %.6lf seconds\n",tc - tb);

        if (mq_close(qdes) == -1) {
	        perror("PRODUCER - mq_close() failed");
	        exit(2);
        }

        if (mq_unlink(qname) != 0) {
            perror("mq_unlink() failed");
            exit(3);
        }
	    
		wait (&child_status);
		if (WIFEXITED (child_status))
			printf("Child Process exited normally\n");
	}
	else 
	{
		//In child process (CONSUMER)
		//printf("CHILD Process ID %d\n", (int)getpid());
        sleep(1);

        if (B < N)
        {
            int i = 0;
            int receivedInt = 0;
	        int ret_open = mq_open(qname, O_RDONLY, mode, &attr);
            //printf("CONSUMER - mq_open: Success! ret_open: %d\n", ret_open);
	        
            while (i < N)
	        { 
                if (mq_receive(ret_open, (char *)&receivedInt, sizeof(int), 0) == -1)
                {
                    perror("CONSUMER - ERROR");
                }
                printf("CONSUMER - Received %d from queue\n",receivedInt);
                fflush(stdout);		        
                i++;
	        }

	        if (mq_close(ret_open) == -1) {
		        perror("CONSUMER - mq_close() failed");
		        exit(2);
	        }
        }
    }
}

int main(int argc, char* argv[]) 
{
    int N = atoi(argv[1]);
    int B = atoi(argv[2]);

	printf("Done Adding N and B. N = %d and B = %d\n", N, B);
    fflush(stdout);	
	        
	producer(N, B);
    
    return 0;
}
