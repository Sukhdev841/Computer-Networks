#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

#define PORT 9000

int main()
{
  int main_socket;
  main_socket = socket(AF_INET,SOCK_STREAM,0);
  cout<<"\nGot socket "<<main_socket<<endl;
  sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(PORT);

  int r = bind(main_socket,(sockaddr *)&addr,sizeof(addr));
  cout<<"\nBinding done with r = "<<r;
  cout<<"\nListening to "<<PORT<<endl;
  cout<<"\nListening to "<<PORT<<endl;
   r = listen(main_socket,3);
   cout<<"\nlisten returns : "<<r<<endl;
  int addrlen;
  cout<<"\nAccepting now\n";
  cout<<"\nAccepting now\n";
  int nsfd = accept(main_socket,(sockaddr*) &addr,(socklen_t*)&addrlen);
  cout<<"\nClient came";
  int c = fork();
  int fd2;
  if( c == 0)
  {
    cout<<"\nChild is going to execute.";
    dup2(nsfd,fd2);
    char buf[100];
    read(fd2,buf,90);
    cout<<"BUF= "<<buf<<endl;
    char **args = NULL;
    cout<<"\nchild: dup done.";
    execv("./d",args);
    exit(0);
  }

  close(nsfd);
  close(main_socket);

  return 0;

}
