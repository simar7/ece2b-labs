#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/stat.h>

// Global Variables
struct timeval time_a, time_b, time_c;

int producer( int N, int B )
{
	mqd_t qdes;
	char qname[] = "/mailbox_s244sing";
	mode_t mode = S_IRUSR | S_IWUSR;
	int qdes_mode = O_RDWR | O_CREAT;
	struct mq_attr attr;

	attr.mq_maxmsg = B;
	attr.mq_msgsize = sizeof(int);
	attr.mq_flags = 0; 	// Blocking queue activate.

	
	if( gettimeofday( &time_a, NULL ) == -1 )
	{
		printf("[producer]: gettimeofday(time_a) failed!\t ERRNO: %d\n", errno);
		exit(1);
	}

	pid_t child_pid;
	child_pid = fork();
	if( child_pid == -1 )
	{
		printf("[producer]: child fork() failed!\t ERRNO: %d\n", errno);
		exit(1);
	}
	
	else if( child_pid != 0)	// This is the parent.	
	{
		qdes = mq_open(qname, O_RDWR | O_CREAT, mode, &attr);
		if( qdes == -1 )
		{
			printf("[producer]: mq_open() failed!\t ERRNO: %d\n", errno);
			exit(1);
		}

		if( gettimeofday( &time_b, NULL ) == -1 )
		{
			printf("[producer]: gettimeofday(time_b) failed!\t ERRNO: %d\n", errno);
			exit(1);
		}

		int msg_count = 0;
		int random_num = 0;

			while( msg_count < N )
			{
				random_num = (rand() % 9);
				if( mq_send( qdes, (char *)&random_num, sizeof(random_num), 0) == -1 )
					printf("[producer]: mq_send failed!\t ERRNO: %d\n", errno);
				else
					printf("[producer]: mq_send worked!\t Tx: %d\n", random_num);
				msg_count++;
			}	

			printf("[producer]: Tx finished!\n");

			if( gettimeofday( &time_c, NULL ) == -1 )
			{
				printf("[producer]: gettimeofday(time_c) failed!\t ERRNO: %d\n", errno);
				exit(1);
			}

			if( mq_close(qdes) == -1 )
			{
				printf("[producer]: mq_close() failed!\t ERRNO: %d\n", errno);
				exit(1);
			}
			
			if( mq_unlink(qname) != 0 )
			{
				printf("[producer]: mq_unlink() failed\t ERRNO: %d\n", errno);
				exit(1);
			}

	}
	else
	{
		qdes = mq_open(qname, O_RDWR | O_CREAT, mode, &attr);
		if( qdes == -1 )
		{
			printf("[consumer]: mq_open() failed!\t ERRNO: %d\n", errno);
			exit(1);
		}

		int msg_count = 0;
		int random_num = 0;
		while( msg_count < B )
		{
			if( mq_receive (qdes, (char*)&random_num, sizeof(int), 0) == -1 )
				printf("[consumer]: mq_receive failed!\t ERRNO: %d\n", errno);
			else
				printf("[consumer]: mq_receive worked!\t Tx: %d\n", random_num);
			msg_count++;
		}	
		
		if( mq_close(qdes) == -1 )
		{
			printf("[consumer]: mq_close() failed!\t ERRNO: %d\n", errno);
			exit(1);
		}
		
	}
	
		return 0;
}	

int main( int argc, char* argv[] )
{
	int return_val = 0;
	return_val = producer( atoi(argv[1]), atoi(argv[2]) );

	if( return_val != 0 )
	{
		printf("[main]: Producer did not finish properly!\n Error Code: %d", return_val);
		return -1;
	}
	else
	{
		printf("[main]: Everything finished fine!\n");
		return 0;
	}
}
