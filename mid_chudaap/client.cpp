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
#include <sys/un.h>


using namespace std;

void handler1(int);
void handler2(int);
bool put_id();
int find_own_index();
void get_shared_socket();


#define size 10
char *sem_name;
key_t key;
int shmid;
int *l;
bool i_am_first = false;
sem_t *sem;
int sock;
int index_;

int main()
{
	cout<<"\nid = "<<getpid()<<endl;
	signal(SIGUSR1,handler1);
	signal(SIGUSR2,handler2);
	sem_name = new char[100];
	cout<<"\nEnter key : ";
	cin>>key;
	cout<<"\nEnter semaphore name : ";
	cin.clear();
	cin.ignore();
	cin>>sem_name;
	cout<<"\nname of semaphroe is : "<<sem_name<<endl;
	shmid = shmget(key,size*sizeof(int),0666);
	cout<<"\nshmid = "<<shmid<<endl;
	if(shmid < 0)
	{
		i_am_first = true;
		cout<<"\nCreating shared memory\n";
		shmid = shmget(key,size*sizeof(int),IPC_CREAT|0777);
		cout<<"\nshmid = "<<shmid<<endl;
		sem = sem_open(sem_name,O_CREAT,0666,1);
		cout<<"\nI am first\n";
		l = (int*)shmat(shmid,NULL,0);
		for(int i=0;i<size;i++)
			l[i] = 0;
		cout<<"\nlist is set\n";
	}
	else
	{
		l = (int*)shmat(shmid,NULL,0);
		sem = sem_open(sem_name,0);			// i am not first
		cout<<"\nI am not first\n";
	}
		
	
	if(!put_id())
	{
		cout<<"\nThere is no more space available in list\n";
		exit(0);
	}
	
	
	if(i_am_first)
	{
		
		sock = socket(AF_INET,SOCK_STREAM,0);
		if( sock < 0)
		{
			cout<<"\nSocket failed \n";
			exit(0);
		}
		sockaddr_in addr;
		addr.sin_family = AF_INET;
		addr.sin_port = htons(9000);
		addr.sin_addr.s_addr = inet_addr("127.0.0.1");
		if( connect(sock,(sockaddr*)&addr,sizeof(addr)) < 0)
		{
			cout<<"\nConnect failed\n";
			exit(0);
		}
		cout<<"\nConnection with server succeded\n";
	}
	else
	{
		// request for socket sharing
		cout<<"\nREquesting\n";
		get_shared_socket();
	}
	if(i_am_first)
	{
		kill(getpid(),SIGUSR1);
	}
	while(true)
	{
		cout<<"\nin while loop : "<<getpid();
		
		sem_wait(sem);
		cout<<endl;
		for(int i=0;i<size;i++)
		{
			cout<<l[i]<<" ";
		}
		sem_post(sem);
		sleep(2);
	}
	
	return 0;
}

bool put_id()
{
	cout<<"\nPtting id\n";
	cout<<sem<<endl;
	
	sem_wait(sem);
	bool flag = false;
	
	cout<<"\nGot semaphore\n";
	for(int i=0;i<size;i++)
	{
		cout<<"\n i = "<<i<<endl;
		cout<<"\nl["<<i<<"] = "<<l[i]<<endl;
		if(l[i] == 0)
		{
			
			cout<<"\nid is put at : "<<i<<endl;
			l[i] = getpid();
			
			index_ = i;
			cout<<"\nPtting id done\n";
			flag = true;
			break;
		}
	}
	cout<<"\nlist after putting is \n";
	for(int i=0;i<size;i++)
	{
		cout<<l[i]<<" ";
	}
	sem_post(sem);
	cout<<"\nPtting id done\n";
	return flag;
}

void handler1(int sig)
{
	cout<<"\nGot signal1\n";
	
	// do operation
	
	char buff[100];
	read(sock,buff,99);
	sleep(1);
	cout<<"\nData from server : "<<buff<<" : "<<getpid()<<endl;
	
	// signal next
	sem_wait(sem);
	if(index_+1 < size && l[index_+1] != 0)
		kill(l[index_+1],SIGUSR1);
	else
		kill(l[0],SIGUSR1);
	cout<<"\nafter kill\n";
	sem_post(sem);
}

void handler2(int sig)
{
	cout<<"\nGot signal2\n";
	int sockfd;
	sockaddr_un cliaddr,servaddr;
			//system("rm socket");
	char *path = "./socket";
	sockfd = socket(AF_UNIX,SOCK_STREAM,0);
	cout<<"\nsockfd = "<<sockfd<<endl;
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sun_family = AF_LOCAL;
	strcpy(servaddr.sun_path,path);
	int bind_ret = bind(sockfd,(sockaddr*) &servaddr,sizeof(servaddr));
	cout<<"\nbind_ret "<<bind_ret<<endl;
	int l_ret = listen(sockfd,10);
	cout<<"\nListening : "<<l_ret<<endl;
	
	socklen_t len = sizeof(cliaddr);
	int connfd = accept(sockfd,(sockaddr*) &cliaddr,&len);
	
	cout<<"\nconnfd = "<<connfd<<endl;
	
	cout<<"\nAccepted "<<connfd<<endl;
		iovec iov[1]={0};

		 const char *str = "file";
		 iov[0].iov_base = (void*) str;
		 iov[0].iov_len = strlen(str)+1;

		 msghdr parent_msg;
		 memset(&parent_msg,0,sizeof(parent_msg));
		 cmsghdr *cmsg;
		 char cmsgbuff[CMSG_SPACE(sizeof(sock))]={};
		 parent_msg.msg_name = NULL;
		 parent_msg.msg_namelen = 0;
		 parent_msg.msg_iov=iov;
		 parent_msg.msg_iovlen=1;
		 parent_msg.msg_control = cmsgbuff;
		 parent_msg.msg_controllen= sizeof(cmsgbuff);

		 cmsg = CMSG_FIRSTHDR(&parent_msg);
		 if(cmsg == NULL)
		 {
		   cout<<"\ncmsg is null in server side.\n";
		 }
		 cmsg->cmsg_level = SOL_SOCKET;
		 cmsg->cmsg_type = SCM_RIGHTS;
		 cmsg->cmsg_len = CMSG_LEN(sizeof(sock));
		 memcpy(CMSG_DATA(cmsg),&sock,sizeof(sock));
		 parent_msg.msg_controllen = cmsg->cmsg_len;

		 if(sendmsg(connfd,&parent_msg,0)< 0 )
		 {
		   cout<<"\nsendmsg() failed\n";
		   exit(0);
		 }
		 
		 cout<<"\nmessage sent\n";
		 
		 close(sockfd);
		 close(connfd);
	
	cout<<"\nSharing is done\n";

	
}

void get_shared_socket()
{
	sem_wait(sem);
	cout<<"\nGot semaphore\n";
	cout<<"\ngoing to kill\n";
	cout<<"\nlist is :\n";
	for(int i=0;i<size;i++)
		cout<<l[i]<<" ";
	cout<<endl;
	if(index_-1 < 0 && l[index_-1] != 0)
		kill(l[index_-1],SIGUSR2);
	else
		kill(l[0],SIGUSR2);
	cout<<"\nafter kill get_shared_socket\n";
	sem_post(sem);
	
	sleep(5);
	int sock_temp;
	sockaddr_un cliaddr,servaddr;
	sock_temp = socket(AF_UNIX,SOCK_STREAM,0);
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sun_family = AF_LOCAL;
	char *path = "./socket";
	strcpy(servaddr.sun_path,path);
	
	if(connect(sock_temp,(sockaddr*)&servaddr,sizeof(servaddr)) < 0)
	{
		cout<<"\nConnection failed of unix_socket\n";
	}
	cout<<"\nConnected \n";
	
	
	int rc;
	  msghdr child_msg;
	  memset(&child_msg,   0, sizeof(child_msg));
	  char cmsgbuf[CMSG_SPACE(sizeof(int))];
	  child_msg.msg_control = cmsgbuf; // make place for the ancillary message to be received
	  child_msg.msg_controllen = sizeof(cmsgbuf);

	  printf("Waiting on recvmsg\n");
	  rc = recvmsg(sock_temp, &child_msg, 0);
	  struct cmsghdr *cmsg = CMSG_FIRSTHDR(&child_msg);
	  if (cmsg == NULL) {
		    printf("cmsg is null\n");
		    exit(0);
	  }
	  if(cmsg -> cmsg_type != SCM_RIGHTS)
	  {
		 cout<<"\nno SCM_RIGHTS\n";
		 exit(0);
	  }
	  memcpy(&sock, CMSG_DATA(cmsg), sizeof(sock));
		cout<<"\nSocket received with value : "<<sock<<endl;
	
}

