#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <pthread.h>
#include <fcntl.h>

using namespace std;

#define size 10
#define main_fifo "main_fifo"
#define PORT 9000

key_t shared_key = 1; 
int fd = -1;
int sock = -1;
int *list;

void handler(int sig)
{
	// do your operation
	
	
	
	// signal next process in list
	for(int i=0;i<size;i++)
	{
		if(list[i] == getpid())
		{
			if( i+1 < size  && list[i+1] != 0)
			{
				kill(list[i+1],SIGUSR1);
				break;
			}
			else
			{
				kill(list[0],SIGUSR1);
				break;
			}
			
		}
	}
}

void connect()
{
	 struct sockaddr_in address;
    int  valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        exit(0); 
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        exit(0);
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        exit(0);
    }
    cout<<"\nsocket connected\n";
}

int main()
{
	signal(SIGUSR1,handler);
	
	int shmid =  shmget(shared_key,size*sizeof(int),0666);
	bool i_am_first = false;
	if(shmid < 0)
	{
		// memory does't exists yet
		shmid = shmget(shared_key,size*sizeof(int),IPC_CREAT|0666);
		cout<<"\nMemory doesn't exists\n";
		i_am_first = true;
	}
	list = (int*)shmat(shmid,NULL,0);
	if(i_am_first)
	{
		// setting everthing to 0
		for(int i=0;i<size;i++)
			list[i]=0;
		mkfifo(main_fifo,0777);
		cout<<"\nfifo successfully created\n";
		connect();
	}
	int i;
	fd = open(main_fifo,O_RDWR);
	cout<<"\nFd = "<<fd<<endl;
	cout<<"\nSetting shared memory\n";
	for(i=0;i<size;i++)
	{
		cout<<"\ni = "<<i<<endl;
		if(*(list+i) == 0)
		{
			*(list+i)= getpid();
			break;
		}
	}
	cout<<"\nMemory done\n";
	if( i== size)
	{
		cout<<"\nNo empty space in list\n";
		exit(0);
	}
	
	if(i_am_first)
	{
		kill(getpid(),SIGUSR1);
	}

	while(true)
	{
		sleep(2);
	}

	
	return 0;	
}

