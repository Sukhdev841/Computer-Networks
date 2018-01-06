#include <unistd.h>
#include <bits/stdc++.h>
#include <sys/types.h>
#include <fcntl.h>

using namespace std;

int main()
{
	int fd[2];
	pipe(fd);
	int c = fork();
	

	if(c > 0)
	{
		dup2(fd[1],1);
		write(1,"stdout_of_p1",13);
	}
	else
	{
		dup2(fd[0],0);
		char **t = NULL;
		execv("./p2",t);
	}

	return 0;
}