#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <mqueue.h>
#include <time.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>

// Global Variables
struct timeval time_a, time_b, time_c;

int producer( int N, int B )
{
	mqd_t qdes;
	int qname[] = "/dev/mqueue/mailbox_s244sing_lab2_producer";
	mode_t mode = S_IRUSR | S_IWUSR;
	int qdes_mode = O_RDWR | O_CREAT;
	struct mq_attr attr;

	attr.mq_maxmsg = B;
	attr.mq_msgsize = sizeof(int);
	attr.mq_flags = 0; 	// Blocking queue activate.

	qdes = mq_open( qname, qdes_mode, mode, &attr );
	if( qdes == -1 )
	{
		printf("[producer]: mq_open() failed!\t ERRNO: %d\n", errno);
		exit(1);
	}
	
	if( gettimeofday( &time_a, NULL ) == -1 )
	{
		printf("[producer]: gettimeofday() failed!\t ERRNO: %d\n", errno);
		exit(1);
	}

	pid_t child_pid;
	child_pid = fork(void);
	if( child_pid == -1 )
	{
		printf("[producer]: child fork() failed!\t ERRNO: %d\n", errno);
		exit(1);
	}
	

int main( int argc, char* argv[] )
{
	int return_val = 0;
	return_val = producer( argv[1], argv[2] );

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
