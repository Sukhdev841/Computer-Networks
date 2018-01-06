#include <semaphore.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#define SEM "/s"

using namespace std;

int main()
{
	sem_t *x = sem_open(SEM,O_CREAT,0644,0);

	cout<<"\nx : waiting\n";
	sem_wait(x);
	cout<<"x : got\n";
	return 0;
}