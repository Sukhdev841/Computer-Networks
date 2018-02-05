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

int p1_pid_t;

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
	//signal(sig,SIG_IGN);
	signal(SIGUSR2,handler);
	//cout<<"\np2: signal set\n";
	char str[6];
	read(0,str,6);
	kill(p1_pid_t,SIGUSR1);
	if(str[0]=='s')
		exit(0);

	cout<<"\np2: "<<str<<endl;

	//cout<<"\np2: kill called\n";
	//cout<<"\np2: kill :"<<kill(p1_pid_t,SIGUSR1);
	//cout<<"\np2: kill executed\n";
	//sleep(1);
	
}

int main()
{
	//cout<<"\np2 : pid = "<<getpid()<<endl;
	signal(SIGUSR2,handler);
	int wfd = open(PIPE1,O_WRONLY);
	int rfd = open(PIPE2,O_RDONLY);

	char pid[100];
	itoa(getpid(),pid);

	write(wfd,pid,strlen(pid)+1);

	// read
	char str[100];
	read(rfd,str,100);
	p1_pid_t = atoi(str);

//	cout<<"\np2 : parent pid = "<<p1_pid_t<<endl;

	while(true)
	{
		//sleep(1);
	}

	return 0;

}