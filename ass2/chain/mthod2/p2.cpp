#include <unistd.h>
#include <bits/stdc++.h>
#include <sys/types.h>
#include <fcntl.h>


using namespace std;

int main()
{
	char str[50];
	//fflush(stdout);
	//cout<<"\np2: enter input : ";
	//cin>>str;
	read(0,str,20);
	cout<<"\ninput of p2  : "<<str<<endl;


	int fd[2];
	pipe(fd);
	int c = fork();
	

	if(c > 0)
	{
		dup2(fd[1],1);
		write(1,"stdout_of_p2",13);
	}
	else
	{
		dup2(fd[0],0);
		char **t = NULL;
		execv("./p3",t);
	}

	return 0;

}