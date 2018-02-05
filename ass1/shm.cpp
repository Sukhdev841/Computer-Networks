#include <bits/stdc++.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>
#include <fcntl.h>

#define SEMX "/semx"
#define SEMY "/semy"

key_t key_x;
key_t key_y;

using namespace std;

struct data
{
	int x;
	int y;
};

void parent()
{
	while(true)
	{
		sem_t *sem_x = sem_open(SEMX,0);
		sem_t *sem_y = sem_open(SEMY,0);
		sem_wait(sem_x);
		int *x;
		int *y;
		int shmid = shmget(key_x,1*sizeof(data),IPC_CREAT | 0666);
		x = (int*)shmat(shmid,NULL,0);

		shmid = shmget(key_y,1*sizeof(data),IPC_CREAT | 0666);

		y = (int*)shmat(shmid,NULL,0);

		cout<<"x = "<<(*y) + 1<<" (parent) \n";
		*x = (*y) + 1;
		sem_post(sem_y);
		sleep(1);
	}
}

void child()
{
	while(true)
	{
		sem_t *sem_x = sem_open(SEMX,0);
		sem_t *sem_y = sem_open(SEMY,0);
		sem_wait(sem_y);
		int *x;
		int *y;
		int shmid = shmget(key_x,1*sizeof(data),IPC_CREAT | 0666);
		x = (int*)shmat(shmid,NULL,0);

		shmid = shmget(key_y,1*sizeof(data),IPC_CREAT | 0666);

		y = (int*)shmat(shmid,NULL,0);

		cout<<"y = "<<(*x) + 1<<" (child) \n";
		*y = (*x) + 1;
		sem_post(sem_x);
		sleep(1);

	}
}

int main()
{
	sem_t *sem_x = sem_open(SEMX,O_CREAT,0644,1);
	sem_t *sem_y = sem_open(SEMY,O_CREAT,0644,0);

	
	int shmid;

	key_x = 1234;
	key_y = 1235;

	shmid = shmget(key_x,1*sizeof(data),IPC_CREAT | 0666);

	int *d = (int*)shmat(shmid,NULL,0);
	*d = 0;

	shmid = shmget(key_y,1*sizeof(data),IPC_CREAT | 0666);

	d = (int*)shmat(shmid,NULL,0);
	*d = 0;

	int c = fork();

	if(c > 0)
	{
		parent();
	}
	else
	{
		child();
	}

	return 0;

}