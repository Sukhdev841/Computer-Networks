#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <bits/stdc++.h>
#include <semaphore.h>
#include <pthread.h>

using namespace std;

#define BUFFER_SIZE 1024
#define SEM "/singh"
char name[30];
int wfd,rfd;

sem_t output;
sem_t *sem;

const char *main_pipe = "fifo";

struct message
{
	char user_name[20];
	char message[1004];
};


void extract_message(message& msg,char *input)
{
	// first character will be type of message so should be 2 in this case
	char temp[100];
	int i;
	for(i=0;i<strlen(input) && input[i] != '$';i++)
	{
		temp[i] = input[i];
	}
	temp[i]='\0';
	strcpy(msg.user_name,temp);
	int j=0;
	i++;
	for(i;i<strlen(input);i++,j++)
	{
		temp[j] = input[i];
	}
	temp[j]='\0';
	strcpy(msg.message,temp);
}


char* to_string(char *message)
{
	char* msg = new char[1024];
	msg[0] = '2';
	strcat(msg,name);
	strcat(msg,"$");
	strcat(msg,message);
	return msg;
}

void *send_message(void *args);

void* server_input(void *args)
{
	while(true)
	{
		char *server_message = new char[1024];
		read(rfd,server_message,1024);
		message msg;
		extract_message(msg,server_message);
		cout<<"\n"<<msg.user_name<<" : "<<msg.message;
		cin.clear();
	}
	return NULL;
}

void* send_message(void *args)
{
	while(true)
	{
		cout<<"\nType msg : ";
		char msg[1024];
		cin>>msg;
		if(!strcmp(msg,"exit"))
		{
			char *msg = new char[1024];
			msg[0] = '3';
			strcat(msg,name);
			strcat(msg,"$");
			strcat(msg,"exiting");
			sem_wait(sem);
			write(wfd,msg,1024);
			cout<<"\nsent as exit message\n";
			delete(msg);
			close(rfd);
			close(wfd);
			cout<<"\nYou did exit from chat.\n";
			exit(0);
			continue;
		}
		sem_wait(sem);
		write(wfd,to_string(msg),1024);
	}
	return NULL;
}

int main()
{
	//cin.ignore();
	//cin.clear();
	sem_init(&output,0,1);

	wfd = open(main_pipe,O_WRONLY);
	char *message = new char[100];
	cout<<"\nEnter user name : ";
	cin>>name;
	strcat(message,"1");
	strcat(message,name);
	strcat(message,"$");
	strcat(message,name);

	//cout<<"\ninfo done : "<<message<<endl;

	mkfifo(name,0666);

	sem = sem_open(SEM,0);
	
	sem_wait(sem);
	write(wfd,message,strlen(message)+1);

	rfd = open(name,O_RDONLY);

	//cout<<"\nPipe opened\n";

	char *server_message = new char[1024];
	read(rfd,server_message,1024);
	cout<<"\n\t\t "<<server_message<<endl<<endl;

	pthread_t server_trd,user_trd;

	pthread_create(&server_trd,NULL,server_input,NULL);
	pthread_create(&user_trd,NULL,send_message,NULL);

	pthread_join(server_trd,NULL);
	pthread_join(user_trd,NULL);

	return 0;


}