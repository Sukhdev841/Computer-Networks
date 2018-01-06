#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <bits/stdc++.h>

using namespace std;

#define SEM "/singh"

#define BUFFER_SIZE 1024

const char *main_pipe = "fifo";

struct user
{
	char name[20];
	int fd;
};

struct message
{
	char user_name[20];
	char message[1004];
	int type;
};

vector<user> all_users;

void extract_message(message& msg,char *input)
{
	// first character will be type of message so should be 2 in this case
	char temp[100];
	int i;
	for(i=1;i<strlen(input) && input[i] != '$';i++)
	{
		temp[i-1] = input[i];
	}
	temp[i-1]='\0';
	strcpy(msg.user_name,temp);
	int j=0;
	i++;
	for(i;i<strlen(input);i++,j++)
	{
		temp[j] = input[i];
	}
	temp[j]='\0';
	strcpy(msg.message,temp);
	msg.type = input[0]-48;
}

char* to_string(message msg)
{
	char *t = new char[100];
	strcat(t,msg.user_name);
	strcat(t,"$");
	strcat(t,msg.message);
	strcat(t,"\n");
	return t;
}

void extract_command(char *cmd,char *input)
{
	cmd = new char[strlen(input)];
	for(int i=1;i<strlen(input);i++)
	{
		cmd[i-1] = input[i];
	}
}

int check_type(char *input)
{
	return (int)((int)(input[0])-48);
}

int register_user(user usr)
{
	all_users.push_back(usr);
	return 1;
}

void send_message(message msg)
{
	for(int i=0;i<all_users.size();i++)
	{
		write(all_users[i].fd,to_string(msg),strlen(to_string(msg))+1);
	}
	cout<<"\nmessage \""<<to_string(msg)<<"\" send to all users\n";
}

int main()
{
	int fd;

	mkfifo(main_pipe,0777);
	sem_t *sem = sem_open(SEM,O_CREAT,0644,1);
	//sem_t *sem = sem_open(SEM,0);
	while(true)
	{
		int fd = open(main_pipe,O_RDONLY);
		
		char *input = new char[1024];

		read(fd,input,1024);
		sem_post(sem);
		//cout<<"\nSemaphore posted\n";
		cout<<"\nInput : "<<input;

		user new_user;
		message new_msg;
		vector<user>::iterator it = all_users.begin();

		extract_message(new_msg,input);

		switch(new_msg.type)
		{
			case 1:
					strcpy(new_user.name,new_msg.user_name);
					new_user.fd = open(new_msg.message,O_WRONLY);
					if(register_user(new_user))
					{
						write(new_user.fd,"you got registered",128);
						cout<<"\nNew User "<<new_user.name<<" registered with pipe "<<new_user.fd<<endl;
						strcpy(new_msg.user_name,"server");
						strcpy(new_msg.message,new_user.name);
						strcat(new_msg.message," is new member to group.");
						send_message(new_msg);
						//send_message("Welcome new user");
					}
					else
					{
						// send message to respective user for failure of registration
						write(new_user.fd,"sorry can't register",128);
					}
					// register
					break;
			case 2:
					// send messgae
					send_message(new_msg);
					break;
			case 3:
					// exit
					// unregister_usr
					
					for(it;it!=all_users.end();it++)
					{
						cout<<endl<<(*it).name<<" == "<<new_msg.user_name;
						if(!strcmp((*it).name,new_msg.user_name))
						{
							strcpy(new_msg.user_name,"server");
							strcpy(new_msg.message,(*it).name);
							strcat(new_msg.message," exits.");
							send_message(new_msg);
							all_users.erase(it);
							cout<<"\nUser deleted\n";
							break;
						}
					}
					break;
			default:
					cout<<"\nMessage type not recognised.\n";
		}
	}


	return 0;
}