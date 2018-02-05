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

int main()
{
	char name[100];
	cout<<"\nEnter pipe name : ";
	cin>>name;
	mkfifo(name,0777);
	int wfd = open(name,O_WRONLY);
	cout<<"\n"<<name<<" opened fifo\n";

	while(true)
	{
		cout<<"\nEnter message : ";
		cin>>name;
		write(wfd,name,strlen(name)+1);
	}

	return 0;
}