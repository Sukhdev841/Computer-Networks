#include <semaphore.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

using namespace std;

#define SEM "/s"

int main()
{
	sem_t *x = sem_open(SEM,0);

	sem_post(x);
	cout<<"\ny: posted\n";
	cout<<"\ny : waiting\n";
	sem_wait(x);
	cout<<"y : got\n";
	return 0;
}