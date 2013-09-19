#include <sys/types.h>
#include <pwd.h>
#include <stdio.h>
#include <errno.h>
#include <grp.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <dirent.h>
#include <time.h>
#include <string.h>

int fileownership( char* str_path )
{
	struct stat buf;
	if (stat(str_path, &buf) < 0)
	{
		perror("stat() failed");
		return -1;
	}
	
	struct passwd* uid_ptr;
	uid_ptr = getpwuid(buf.st_uid);

	struct group*  gid_ptr;
	gid_ptr = getgrgid(buf.st_gid);

	printf("%s\t%s\t", uid_ptr->pw_name, gid_ptr->gr_name);

	return 0;
}

int filepermissions(char* str_path)
{
	char str[] = "---------";
	struct stat buf;

    if( lstat(str_path, &buf) < 0)
	{	
        perror("lstat failed on file\n");
        return errno;
    }	
    
    mode_t mode = buf.st_mode;
	str[0] = (mode & S_IRUSR) ? 'r' : '-';
    str[1] = (mode & S_IWUSR) ? 'w' : '-';
	str[2] = (mode & S_IXUSR) ? 'x' : '-';
	str[3] = (mode & S_IRUSR) ? 'r' : '-';
    str[4] = (mode & S_IWGRP) ? 'w' : '-';
	str[5] = (mode & S_IXGRP) ? 'x' : '-';
	str[6] = (mode & S_IROTH) ? 'r' : '-';
    str[7] = (mode & S_IWOTH) ? 'w' : '-';
	str[8] = (mode & S_IXOTH) ? 'x' : '-';

	printf("%s\t", str);

	char* ptr;
    if( S_ISREG(buf.st_mode)) 	ptr = "regular file";
	else if (S_ISDIR(buf.st_mode))  ptr = "directory";
	else if (S_ISCHR(buf.st_mode))  ptr = "character special";
	else if (S_ISBLK(buf.st_mode))  ptr = "block special";
	else if (S_ISFIFO(buf.st_mode)) ptr = "fifo";
#ifdef S_ISLNK
	else if (S_ISLNK(buf.st_mode))  ptr = "symbolic link";
#endif
#ifdef S_ISSOCK
	else if (S_ISSOCK(buf.st_mode)) ptr = "socket";
#endif
	else    
        ptr = "unknown file bro :(";
	
	printf("%s\t", ptr);
	return 0;
}

int filetimes(char* str_path)
{

    struct stat buf;
	if (lstat(str_path, &buf) < 0)
	{
		perror("lstat failed");
	    return errno;
    }
	
	time_t accesstime = buf.st_atime;
	char* timeptr = ctime(&accesstime);
	
    printf("Access Time: %s", timeptr);

    time_t modifytime = buf.st_mtime;
	timeptr = ctime(&modifytime);
	printf("Modify Time: %s", timeptr);
		
	time_t statuschangetime = buf.st_ctime;
	timeptr = ctime(&statuschangetime);
	printf("Status Time: %s", timeptr);

    return 0;
}

int filesizes(char* str_path)
{
    struct stat buf;
    if (lstat(str_path, &buf) < 0)
    {
        perror("stat() failed");
        return errno;
    }

    off_t filesize = buf.st_size;
    printf("%5llub\t", (long long unsigned)filesize);
    printf("%s\n", str_path);

    return 0;
}

int main( int argc, char* argv[] )
{
	DIR *p_dir;
	struct dirent *p_dirent;
   
	/* Temporarily commented out to deal with automated testing 
	if( (argc < 2) || ( strcmp( argv[1], "-h") == 0 )  || (argc > 3) )
    {
        printf("Usage: %s <dir name>\n", argv[0]);
        printf("-h \t Show help\n");
        printf("-vvv \t Be Very Verbose (default)\n");
        printf("-P \t Be Precise\n");
        exit(1);
    }
	*/

	if( (p_dir = opendir(".")) == NULL )
	{
		printf("opendir() failed\n");
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

		if( filepermissions(str_path) != 0 )	
			printf("[filepermissions]: Something went wrong! Errno: %d\n", errno);

		if( fileownership(str_path) != 0 )
			printf("[fileownership]: Something went wrong! Errno: %d\n", errno);

		if( filesizes(str_path) != 0 )
			printf("[filesizes]: Something went wrong! Errno: %d\n", errno);

	    if( filetimes(str_path) != 0 )
				printf("[filetimes]: Something went wrong! Errno: %d\n", errno);
		
    }

	closedir(p_dir);
    
	return 0;
}	
