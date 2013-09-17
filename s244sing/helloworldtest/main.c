#include <sys/types.h>
#include <pwd.h>
#include <stdio.h>
#include <errno.h>
#include <grp.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <dirent.h>

int fileownership( char* str_path )
{
	struct stat buf;
	if (stat(str_path, &buf) < 0)
	{
		perror("stat() failed");
		return -1;
	}
	
	gid_t grpid = buf.st_gid;
	uid_t usrid = buf.st_uid;

	printf("%5d\t", grpid);
	printf("%5d\t", usrid);

	struct passwd* uid_ptr;
	uid_ptr = getpwuid(buf.st_uid);

	printf("uid_ptr->pw_name = %s", uid_ptr->pw_name);

	return 0;
}

int main( int argc, char* argv[] )
{
	DIR *p_dir;
	struct dirent *p_dirent;

	if( (p_dir = opendir(argv[1])) == NULL )
	{
		printf("opendir(%s) failed\n", argv[1]);
		exit(1);
	}

	while( (p_dirent = readdir(p_dir)) != NULL )
	{
		char *str_path = p_dirent->d_name;
		
		if( str_path == NULL )
		{
			perror("Null pointer found!\n");
			exit(2);
		}

		if( fileownership(str_path) != 0 )
			printf("Something went wrong! Errno: %d\n", errno);
	
	}

	return 0;
}	
