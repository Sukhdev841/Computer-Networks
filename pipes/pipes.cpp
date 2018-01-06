#include <iostream>
#include <unistd.h>

using namespace std;

void parent(int rfd,int wfd)
{
	while(true)
	{
		const char *s = "parent";
		char *k = new char[100];
		write(wfd,s,strlen(s));
		read(rfd,k,100);
		cout<<k<<endl;
		sleep(1);
	}
}

void child(int rfd,int wfd)
{
while(true)
	{
		const char *s = "child";
		char *k = new char[100];
		write(wfd,s,strlen(s));
		read(rfd,k,100);
		cout<<k<<endl;
		sleep(1);
	}
}

int main()
{
	int pfd1[2],pfd2[2];
	
	if(pipe(pfd1))
	{
		cout<<"\nPipe creation failed (1)\n";
		exit(0);
	}
	
	if(pipe(pfd2))
	{
		cout<<"\nPipe creation failed (2)\n";
		exit(0);
	}
	
	int c = fork();
	
	if(c>0)
	{
		// parent process
		close(pfd1[0]);
		close(pfd2[1]);
		parent(pfd2[0],pfd1[1]);
	}
	
	else
	{
		//child process
		close(pfd1[1]);
		close(pfd2[0]);
		child(pfd1[0],pfd2[1]);
	}
	
	return 0;
	
}
