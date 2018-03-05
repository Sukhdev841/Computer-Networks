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

char *SEM;
#define size 10
#define main_fifo "main_fifo"
#define PORT 9000

key_t shared_key = 2; 
int fd = -1;
int sock = -1;
int *list_;
sem_t *sem;
bool i_am_first = false;

void handler1(int sig);
void handler2(int sig);
void connect();
void create_unix_socket();
void share_fd(int sock);
void receive_fd(int& sock);

int main()
{
	signal(SIGUSR1,handler1);
	signal(SIGUSR2,handler2);
	
	cout<<"\nEnter key : ";
	cin>>shared_key;
	cout<<"\nEnter semaphore name ; ";
	SEM = new char[100];
	cin.clear();
	cin.ignore();
	cin>>SEM;
	
	int shmid =  shmget(shared_key,size*sizeof(int),0666);
	i_am_first = false;
	if(shmid < 0)
	{
		// memory does't exists yet
		shmid = shmget(shared_key,size*sizeof(int),IPC_CREAT|0666);
		cout<<"\nMemory doesn't exists\n";
		i_am_first = true;
	}
	list_ = (int*)shmat(shmid,NULL,0);
	if(i_am_first)
	{
		// setting everthing to 0
		for(int i=0;i<size;i++)
			list_[i]=0;
		connect();
		sem = sem_open(SEM,O_CREAT,0644,1);
		
	}
	else
	{
		sem = sem_open(SEM,0);
	}
	int i;
	
	cout<<"\nFd = "<<fd<<endl;
	cout<<"\nSetting shared memory\n";
	for(i=0;i<size;i++)
	{
		cout<<"\ni = "<<i<<endl;
		if(*(list_+i) == 0)
		{
			*(list_+i)= getpid();
			break;
		}
	}
	cout<<"\nMemory done\n";
	if( i== size)
	{
		cout<<"\nNo empty space in list\n";
		exit(0);
	}
	
	
	if( !i_am_first)
	{
		// receive socket from ur previous process
		kill(list_[i-1],SIGUSR2);
		sleep(2);
		receive_fd(sock);
		cout<<"\nsocket recived by sharing = "<<sock<<endl;
	}
	
	
	
	
	
	
	
	
	kill(getpid(),SIGUSR1);

	while(true)
	{
		sleep(2);
	}

	
	return 0;	
}

void handler2(int sig)
{
	// get id of process who signals 
	cout<<"\nGot signal2 "<<getpid()<<endl;
	sem_wait(sem);
	
	// signal next process in list
	int prev_id = -1;
	for(int i=0;i<size;i++)
	{
		if(list_[i] == getpid())
		{
			if( i+1 < size  && list_[i+1] != 0)
			{
				prev_id = list_[i+1];
				break;
			}
			else
			{
				prev_id = list_[0];
				break;
			}
		}
	}
	cout<<"\ngot own id\n";
	
	// prev_id signals me
	// send him socket through unix socket
	cout<<"\nCreating unix socket\n";
	create_unix_socket();
	cout<<"\nUnix socket creation done\n";
	int client_fd = accept(fd,NULL,NULL);
	
	cout<<"\nClient accepted\n";
	
	share_fd(client_fd);
	cout<<"\nFd is sent\n";
	
	close(fd);		// closing unix socket
	
	// now accept that socket
	
	sem_post(sem);
	
}

void share_fd(int sock)
{
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
		int ret_code = sendmsg(fd,&parent_msg,0);
    if(ret_code < 0 )
    {
      cout<<"\nsendmsg() failed ret_code "<<ret_code;
      exit(0);
    }
    
    cout<<"\nSocket sent by "<<getpid()<<endl;
}

void receive_fd(int &sock)
{
	  int rc;
	  msghdr child_msg;
	  memset(&child_msg,   0, sizeof(child_msg));
	  char cmsgbuf[CMSG_SPACE(sizeof(int))];
	  child_msg.msg_control = cmsgbuf; // make place for the ancillary message to be received
	  child_msg.msg_controllen = sizeof(cmsgbuf);

	  printf("Waiting on recvmsg\n");
	  rc = recvmsg(sock, &child_msg, 0);
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
	  printf("Received descriptor = %d\n", sock);
	  cout<<"\nthis is process "<<getpid()<<endl;
}

void handler1(int sig)
{
	// do your operation
	
	cout<<"\nGot signal1\n";
	sem_wait(sem);
	
	// signal next process in list
	for(int i=0;i<size;i++)
	{
		if(list_[i] == getpid())
		{
			if( i+1 < size  && list_[i+1] != 0)
			{
				kill(list_[i+1],SIGUSR1);
				break;
			}
			else
			{
				kill(list_[0],SIGUSR1);
				break;
			}
			
		}
	}
	
	sem_post(sem);
	
	// now send shared_socket to net process

    
    
    sleep(2);

}

void create_unix_socket()
{
	// Create a directory with the proper permissions
	char path[100];
	path[0]='.';
	path[1]='\0';
	cout<<"\ncreating directory\n";
	mkdir(path, 0777);
	cout<<"\ndirectory created successfully\n";
	// Append the name of the socket
	strcat(path, "/socket_name");
	cout<<"\nafter catenation path = "<<path<<endl;

	// Create the socket normally
	sockaddr_un address;
	address.sun_family = AF_UNIX;
	cout<<"\nCopying path\n";
	strcpy(address.sun_path, path);
	cout<<"\npath copy is done\n";
	cout<<"\nexecuting unix socket\n";
	int fd = socket(AF_UNIX, SOCK_STREAM, 0);
	cout<<"\nunix socket fd = "<<fd<<endl;
	cout<<"\nexecuting bind\n";
	bind(fd, (sockaddr*)(&address), sizeof(address));
	cout<<"\nunix socket bind is done\n";
	listen(fd, 1);	
	cout<<"\nnow listening\n";
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
    cout<<"\nsocket connected with server\n";
}

ssize_t write_fd(int fd, void *ptr, size_t nbytes, int sendfd)
{
    struct msghdr   msg;
    struct iovec    iov[1];

#ifdef  HAVE_MSGHDR_MSG_CONTROL
    union {
      struct cmsghdr    cm;
      char              control[CMSG_SPACE(sizeof(int))];
    } control_un;
    struct cmsghdr  *cmptr;

    msg.msg_control = control_un.control;
    msg.msg_controllen = sizeof(control_un.control);

    cmptr = CMSG_FIRSTHDR(&msg);
    cmptr->cmsg_len = CMSG_LEN(sizeof(int));
    cmptr->cmsg_level = SOL_SOCKET;
    cmptr->cmsg_type = SCM_RIGHTS;
    *((int *) CMSG_DATA(cmptr)) = sendfd;
#else
    msg.msg_accrights = (caddr_t) &sendfd;
    msg.msg_accrightslen = sizeof(int);
#endif

    msg.msg_name = NULL;
    msg.msg_namelen = 0;

    iov[0].iov_base = ptr;
    iov[0].iov_len = nbytes;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    return(sendmsg(fd, &msg, 0));
}

ssize_t
read_fd(int fd, void *ptr, size_t nbytes, int *recvfd)
{
    struct msghdr   msg;
    struct iovec    iov[1];
    ssize_t         n;
    int             newfd;

#ifdef  HAVE_MSGHDR_MSG_CONTROL
    union {
      struct cmsghdr    cm;
      char              control[CMSG_SPACE(sizeof(int))];
    } control_un;
    struct cmsghdr  *cmptr;

    msg.msg_control = control_un.control;
    msg.msg_controllen = sizeof(control_un.control);
#else
    msg.msg_accrights = (caddr_t) &newfd;
    msg.msg_accrightslen = sizeof(int);
#endif

    msg.msg_name = NULL;
    msg.msg_namelen = 0;

    iov[0].iov_base = ptr;
    iov[0].iov_len = nbytes;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

    if ( (n = recvmsg(fd, &msg, 0)) <= 0)
        return(n);

#ifdef  HAVE_MSGHDR_MSG_CONTROL
    if ( (cmptr = CMSG_FIRSTHDR(&msg)) != NULL &&
        cmptr->cmsg_len == CMSG_LEN(sizeof(int))) {
        if (cmptr->cmsg_level != SOL_SOCKET)
            err_quit("control level != SOL_SOCKET");
        if (cmptr->cmsg_type != SCM_RIGHTS)
            err_quit("control type != SCM_RIGHTS");
        *recvfd = *((int *) CMSG_DATA(cmptr));
    } else
        *recvfd = -1;       /* descriptor was not passed */
#else
/* *INDENT-OFF* */
    if (msg.msg_accrightslen == sizeof(int))
        *recvfd = newfd;
    else
        *recvfd = -1;       /* descriptor was not passed */
/* *INDENT-ON* */
#endif

    return(n);
}

