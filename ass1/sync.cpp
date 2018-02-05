// using semaphores
// using threads

#include <bits/stdc++.h>
#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

using namespace std;

int x,y;
sem_t sem_x,sem_y;

void *process1(void* arg)
{
     while(true)
     {
     	  sem_wait(&sem_x);
          x = y+1;
          cout<<"\nx = "<<x<<endl;
          sem_post(&sem_y);
          sleep(1);
     }
}

void *process2(void *arg)
{
	while(true)
     {
     	  sem_wait(&sem_y);
          y = x+1;
          cout<<"\ny = "<<y<<endl;
          sem_post(&sem_x);
          sleep(1);
     }
}

int main()
{
     sem_init(&sem_x,0,1);
     sem_init(&sem_y,0,0);

     x = 0;
     y = 1;

     pthread_t p1,p2;

     pthread_create(&p1,NULL,process1,NULL);
     pthread_create(&p2,NULL,process2,NULL);

     pthread_join(p1,NULL);
     pthread_join(p2,NULL);

     return 0;
}	
