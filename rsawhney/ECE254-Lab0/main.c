/**
 * @file: $HOME/ece254/Lab0/starter/main.c
 * @brief: programming a simple ls command
 * @author: rsawhney@uwaterloo.ca
 * @date: 2013/09/11
 */

#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main (int argc, char *argv[])
{
	DIR *p_dir;
	struct dirent *p_dirent;
	struct stat buf;	

	if (argc < 2) 
	{
		printf("Usage: $HOME/ece254/Lab0\n");
		exit (1);
	}

	else if ((p_dir = opendir(argv[1])) == NULL) 
	{
		printf("opendir(%s) failed\n", argv[1]);
		exit (1);
	}

	else
	{
		while ((p_dirent = readdir(p_dir)) != NULL) 
		{
			printf ("\n-----------\n");
			
			char *str_name = p_dirent->d_name;
			printf ("Name: %s\n", str_name); 
			
			stat(str_name, &buf);
			printf ("Size: %d\n", buf.st_size);
			
			printf("Permissions: \t");
			printf( (S_ISDIR(buf.st_mode)) ? "d" : "-");
   			printf( (buf.st_mode & S_IRUSR) ? "r" : "-");
   			printf( (buf.st_mode & S_IWUSR) ? "w" : "-");
   			printf( (buf.st_mode & S_IXUSR) ? "x" : "-");
    			printf( (buf.st_mode & S_IRGRP) ? "r" : "-");
    			printf( (buf.st_mode & S_IWGRP) ? "w" : "-");
    			printf( (buf.st_mode & S_IXGRP) ? "x" : "-");
			printf( (buf.st_mode & S_IROTH) ? "r" : "-");
    			printf( (buf.st_mode & S_IWOTH) ? "w" : "-");
   			printf( (buf.st_mode & S_IXOTH) ? "x" : "-");		
		
			
			printf("\nType: \t");
			switch (buf.st_mode & S_IFMT) 
			{
				case S_IFDIR: 
					printf("directory\n");
					break;
				case S_IFREG:
					printf("regular file\n");
					break;
				default:
					printf("unknown file\n");
					break;
			}
			
			printf("Last file access: %s", ctime(&buf.st_atime));
			printf("Last file modification: %s", ctime(&buf.st_mtime));
			printf("Last status change: %s", ctime(&buf.st_ctime));
			
            		struct passwd *uid_struct;            		uid_struct = getpwuid(buf.st_uid);                       		struct group *gid_struct;                        		gid_struct = getgrgid(buf.st_gid);                        		printf("GID: %s\n", gid_struct->gr_name);			printf("UID: %s\n", uid_struct->pw_name);            		printf ("%s\t", str_name); 
		}
	}
	return 0;
}
