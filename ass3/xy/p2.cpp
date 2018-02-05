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
	cout<<"\nin handler of p2\n";
	int shmid_x = shmget(key_x,1*sizeof(int),IPC_CREAT|0666);
	int shmid_y = shmget(key_y,1*sizeof(int),IPC_CREAT|0666);

	int *x = (int*)shmat(shmid_x,NULL,0);
	int *y = (int*)shmat(shmid_y,NULL,0);

	*y = (*x) + 1;

	cout<<"\np2 : x = "<<*x<<" y = "<<*y<<endl;
	signal(SIGUSR2,handler);
	kill(getppid(),SIGUSR1);

}

int main()
{
	cout<<"\nin p2\n";
	signal(SIGUSR2,handler);
	cout<<"\np2: signal done\n";
	while(1);
	return 0;

}