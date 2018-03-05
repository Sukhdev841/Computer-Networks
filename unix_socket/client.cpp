#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <sys/un.h>

#define path "./socket"

using namespace std;

int main()
{
	int sock;
	sockaddr_un cliaddr,servaddr;
	sock = socket(AF_LOCAL,SOCK_STREAM,0);
	bzero(&servaddr,sizeof(servaddr));
	servaddr.sun_family = AF_LOCAL;
	strcpy(servaddr.sun_path,path);
	
	connect(sock,(sockaddr*)&servaddr,sizeof(servaddr));
	
	
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
	  int pass_sd;
	  memcpy(&pass_sd, CMSG_DATA(cmsg), sizeof(pass_sd));
	  printf("Received descriptor = %d\n", pass_sd);
	  char buff[100];
	  read(pass_sd,buff,4);
	  cout<<"\nbuffer = "<<buff<<endl;

	
	return 0;
}
