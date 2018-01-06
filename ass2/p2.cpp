#include <bits/stdc++.h>
#include <unistd.h>
#include <semaphore.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>

using namespace std;

#define SEM1 "/sem_12"
#define SEM2 "/sem_13"

int main()
{

	int fd;
	dup2(0,fd);
	char str[20];
	//read(fd,str,20);
	//fd = 3;
	//fflush(stdout);

	sem_t *s1 = sem_open(SEM1,0);
	sem_t *s2 = sem_open(SEM2,0);
	//cout<<"\np2: after semaphores \n";
	int count = 0;
	//cout<<"\np2: s = "<<s<<endl;
	cout<<"\np2 : fd = "<<fd<<endl;
	cout<<"\np2: starting while loop\n";
	while(true)
	{
		cout<<"\np2: Waiting for semaphore p2\n";
		sem_wait(s2);
		cout<<"\np2 : got semaphroe\n";
		char *buffer = new char[50];
		cin>>buffer;
		//read(fd,buffer,6);
		cout<<"\nP2 : "<<buffer<<endl;
		sem_post(s1);
		cout<<"\np2: s posted\n";

		if(strlen(buffer) == 0)
		{
			cout<<"\np2: end of file reached\n";
			break;
		}
	}

	sem_close(s1);
	sem_close(s2);

	return 0;

}