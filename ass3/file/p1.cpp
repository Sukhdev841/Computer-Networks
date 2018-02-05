#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <bits/stdc++.h>
#include <signal.h>
#include <string.h>
#include <cctype>
#include <cstring>

using namespace std;

#define PIPE1 "pipe1"
#define PIPE2 "pipe2"

int p2_pid_t;
int fd;

void strrev(char *s1,char *s2)
{
	int i =0;
	int j = strlen(s1)-1;
	while(j>=0)
	{
		s2[i] = s1[j];
		i++;
		j--;
	}
	s2[strlen(s1)]='\0';
}

void itoa(int i,char *x)
{
	//cout<<"\nNumber is : "<<i<<endl;
	int j =0;
	while(i)
	{
		int d = i%10;
		i /= 10;
		x[j] = (char)(d+48);
		j++;
	}
	x[j] = '\0';
	char temp[100];
	//cout<<"\nBefore reversing : "<<x<<endl;
	strrev(x,temp);
	strcpy(x,temp);
	//cout<<"\nstring is : "<<x<<endl;
}

void handler(int sig)
{
	signal(sig,SIG_IGN);
	signal(SIGUSR1,handler);
	char str[6];
	read(fd,str,6);
	kill(p2_pid_t,SIGUSR2);
	if(str[0]=='s')
		exit(0);

	cout<<"\np1: "<<str<<endl;
}

void parent()
{
	//cout<<"\nparent pid : "<<getpid()<<endl;
	signal(SIGUSR1,handler);
	mkfifo(PIPE1,0777);
	mkfifo(PIPE2,0777);

	int rfd = open(PIPE1,O_RDONLY);
	int wfd = open(PIPE2,O_WRONLY);

	// first get pid

	char p2_pid[100];
	read(rfd,p2_pid,99);

	// write

	char *pid = new char[100];

	itoa(getpid(),pid);

	

	write(wfd,pid,strlen(pid)+1);
	// first p1 will read lines from file

	char str[6];
	read(fd,str,6);
	cout<<"\np1 : "<<str<<endl;
	p2_pid_t = atoi(p2_pid);

	//	cout<<"\np1: p2pid = "<<p2_pid_t<<endl;

	kill(p2_pid_t,SIGUSR2);

	while(true)
	{
		//sleep(1);
	}

	
}

void child()
{
	dup2(fd,0);
	char **t = NULL;
	//cout<<"\nprocess p2 created\n";
	execv("./p2",t);
}

int main()
{
	fd = open("readme.txt",O_RDONLY);

	int c = fork();
	//int c = 1;

	// int num = 123;
	// char temp[100];
	// itoa(num,temp);
	// cout<<"\ntemp : "<<temp<<endl;
	if(c>0)
	{
		parent();
	}
	else
	{
		child();
	}
	return 0;

}