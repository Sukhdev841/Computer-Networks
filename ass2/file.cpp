#include <bits/stdc++.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/wait.h>

using namespace std;

int fd;
#define SEM1 "/sem_12"
#define SEM2 "/sem_13"

sem_t *s1;
sem_t *s2;

void parent()
{
	while(true)
	{
		cout<<"\nparent: waiting for s1\n";
		sem_wait(s1);
		cout<<"\nparent: got s";
		char *buffer = new char[50];
		read(fd,buffer,6);
		if(strlen(buffer) == 0)
		{
			cout<<"\nparent: end of file reached\n";
			break;
		}
		//cin>>buffer;
		cout<<"\nParent : "<<buffer;
		sem_post(s2);
		cout<<"\nParent: s2 posted by parent";
	}
	sem_close(s1);
	sem_close(s2);
}

void child()
{	

	//cout<<"\nfd in child = "<<fd<<endl;
	dup2(fd,0);
	char str[50];
	//read(0,str,12);
	//cin>>str;
	//cout<<"\nLine read by child : "<<str<<endl;
	//cout<<"\nstdin in child "<<stdin<<endl;

	char **t = NULL;
	//cout<<"\nchild is executing p2\n ";
	execv("./p2",t);
}

int main()
{
	s1 = sem_open(SEM1,O_CREAT,0777,1);
	s2 = sem_open(SEM2,O_CREAT,0777,0);


	fd = open("readme.txt",O_RDONLY);
	//cout<<"\nFD in main = "<<fd<<endl;


	int c ;
	c = fork();

	//dup2(fd,1);

	if( c > 0)
	{
		// parent
		parent();
		//child();
	}
	else
	{
		//child
		child();
	}

	return 0;

}