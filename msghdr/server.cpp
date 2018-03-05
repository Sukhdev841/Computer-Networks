#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include <bits/stdc++.h>

using namespace std;

#define PORT 8080

static const int parent_sock = 1;
static const int child_sock = 0;

void child(int sock)
{
  // char buff[100];
  // memset(&buff,'\0',sizeof(buff));
  // read(sock,buff,sizeof(buff));
  // cout<<"\nRead by child : "<<buff<<endl;
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
  write(pass_sd,"hello by child",14);
}

int main()
{
  int fd[2];
  socketpair(PF_LOCAL,SOCK_STREAM,0,fd);

  int c =fork();
  if(c > 0 )
  {
    close(fd[child_sock]);
  }
  else
  {
    close(fd[parent_sock]);
    child(fd[child_sock]);
    exit(0);
  }

  // server is running
  int server_fd, new_socket;
  struct sockaddr_in address;
  int addrlen = sizeof(address);
  char buffer[1024] = {0};
  char *hello = "Hello from server";

  // Creating socket file descriptor
  server_fd = socket(AF_INET, SOCK_STREAM, 0);

  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons( PORT );

  // Forcefully attaching socket to the port 8080
  bind(server_fd, (struct sockaddr *)&address,sizeof(address));
  listen(server_fd, 3);
  cout<<"\nParent waiting form accpet\n";
  new_socket = accept(server_fd, (struct sockaddr *)&address,(socklen_t*)&addrlen);
  cout<<"\nParent got new client\n";

    // now send new_socket to child
	
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
    cout<<"\nnew_socket = "<<new_socket<<endl;
		int ret_2 = sendmsg(fd[parent_sock],&parent_msg,0);
		cout<<"\nAfter sendmsg : "<<ret_2<<endl;
    if( ret_2 < 0 )
    {
      cout<<"\nsendmsg() failed\n";
      exit(0);
    }

    cout<<"\nSEnd msg done\n";
    // while(1)
    // {
    //   sleep(10);
    // }

  return 0;
}
