#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include <iostream>
#include <string.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>

using namespace std;

#define main_pipe "main_pipe"

pollfd wfd[10];
int count ;
int rfd;

void* reading(void *args)
{
	int i = 0;
	while(true)
	{	
		cout<<"\nCount = "<<count<<endl;
		if(count!=0)
		i = (i+1)%count;
		else
			i=0;
		int rv = poll(wfd,count,3500);
		char *buf = new char[100];
		//reading from all wfds
		if(rv == 0)
		{
			cout<<"\nTime out\n";
		}
		else if (wfd[0].revents & POLLIN) {
            printf("return hit\n");
            read(wfd[0].fd, buf, 10);
        }   
		
	}
}

void* adding_new(void *args)
{
	while(true)
	{
		// adding new user
		char str[100];
		read(rfd,str,99);
		cout<<"\nserver: read from main fifo : "<<str<<endl;
		int fd = open(str,O_RDONLY);
		pollfd x;
		x.fd = fd ;
		x.events = POLLIN;
		wfd[count] = x;
		count++;
		cout<<"\n pushed in fds successfully added new pipe\n";
	}
}

int main()
{
	cout<<"\nserver\n";
	mkfifo(main_pipe,0666);
	rfd = open(main_pipe,O_RDONLY);

	cout<<"\nclient came\n";

	pthread_t t1,t2;

	pthread_create(&t1,NULL,reading,NULL);
	pthread_create(&t2,NULL,adding_new,NULL);

	pthread_join(t1,NULL);
	pthread_join(t2,NULL);

	return 0;
}


// int main(int argc, char *argv[]) {
//     int a;
//     int b;
//     int c;
//     char buf[10];
//     int i;
//     struct pollfd ufds[1];      
//     ufds[0].fd = 0;
//     ufds[0].events = POLLIN;
//     int rv;
//     int dummy;
//     for(i=0; i < 10; i++) {
//         printf("%i ", i);
//         if((rv = poll(ufds, 1, 3500)) == -1) perror("select");
//         else if (rv == 0) printf("Timeout occurred!\n");
//         else if (ufds[0].revents & POLLIN) {
//             printf("return hit\n");
//             read(0, buf, 10);
//         }   
//         fflush(stdout); 
//     }   
//     return 0;
// }