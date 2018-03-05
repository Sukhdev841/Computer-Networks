#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <sys/un.h>
#include <fcntl.h>

#define path "./socket"

using namespace std;

int main()
{
	int sockfd;
	sockaddr_un cliaddr,servaddr;
	sockfd = socket(AF_LOCAL,SOCK_STREAM,0);
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sun_family = AF_LOCAL;
	strcpy(servaddr.sun_path,path);
	bind(sockfd,(sockaddr*) &servaddr,sizeof(servaddr));
	listen(sockfd,1);
	int new_socket = open("file.txt",O_RDONLY);
	
	while(true)
	{
		socklen_t len = sizeof(cliaddr);
		int connfd = accept(sockfd,(sockaddr*) &cliaddr,&len);
		if(connfd < 0)
		{
			cout<<"\nError in accept"<<connfd<<endl;
			exit(0);
		}
		cout<<"\nAccepted "<<connfd<<endl;
		iovec iov[1]={0};

		 const char *str = "file";
		 iov[0].iov_base = (void*) str;
		 iov[0].iov_len = strlen(str)+1;

		 msghdr parent_msg;
		 memset(&parent_msg,0,sizeof(parent_msg));
		 cmsghdr *cmsg;
		 char cmsgbuff[CMSG_SPACE(sizeof(new_socket))]={};
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
		 cmsg->cmsg_len = CMSG_LEN(sizeof(new_socket));
		 memcpy(CMSG_DATA(cmsg),&new_socket,sizeof(new_socket));
		 parent_msg.msg_controllen = cmsg->cmsg_len;

		 if(sendmsg(connfd,&parent_msg,0)< 0 )
		 {
		   cout<<"\nsendmsg() failed\n";
		   exit(0);
		 }
		 
		 cout<<"\nmessage sent\n";

		
	}
	return 0;
}
