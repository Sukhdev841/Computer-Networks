#include <bits/stdc++.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <fcntl.h>

using namespace std;

key_t key_x = 123;
key_t key_y = 124;
int c;

void handler(int sig)
{
	cout<<"\nHandler of p1\n";
	int shmid_x = shmget(key_x,1*sizeof(int),IPC_CREAT|0666);
	int shmid_y = shmget(key_y,1*sizeof(int),IPC_CREAT|0666);

	int *x = (int*)shmat(shmid_x,NULL,0);
	int *y = (int*)shmat(shmid_y,NULL,0);

	*x = (*y) + 1;

	cout<<"\np1 : x = "<<*x<<" y = "<<*y<<endl;
	signal(SIGUSR1,handler);
	kill(c,SIGUSR2);

}

void child()
{
	cout<<"\nin child\n";
	char **t = NULL;
	execv("./p2",t);
	cout<<"\ngoing out of child\n";
}

void parent()
{
	signal(SIGUSR1,handler);
	cout<<"\nin parent\n";
	sleep(3);
	kill(c,SIGUSR2);
	cout<<"\ngoing out parent\n";
	while(1);
}

int main()
{
	int shmid;
	shmid = shmget(key_x,1*sizeof(int),IPC_CREAT | 0666);
	int *d = (int*)shmat(shmid,NULL,0);
	*d = 0;
	shmid = shmget(key_y,1*sizeof(int),IPC_CREAT|0666);
	d = (int*) shmat(shmid,NULL,0);
	*d = 0;

	 c = fork();

	if(c > 0)
	{
		parent();
	}	
	else
	{
		child();
	}

}