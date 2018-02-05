#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <sys/select.h>
#include <pthread.h>
#include "singh_message.h"

using namespace std;

message_t received_msg;
char *received_str;
message_t global_server_msg;

void* service1(void *args)
{
  int sock_fd = *((int*)args);
  message_t service_msg,received_msg;
  strcpy(service_msg.client_name,"service1");
  strcpy(service_msg.message,"this is service1");
  char *service_msg_str = new char[100];
  cout<<"\nService is going to start.\n";
  ofstream fout;
  fout.open("service1.log");
  while(true)
  {
    receive_message(sock_fd,received_msg,&service_msg_str);
    fout<<"\nMessage Received.\n";
    received_msg.print(fout);
    sock_send_message(sock_fd,service_msg);
    fout<<"\nMessage Sent.";
    service_msg.print(fout);
  }
  fout.close();
  return NULL;
}

void *service2(void *args)
{
  int sock_fd = *((int*)args);
  message_t service_msg,received_msg;
  strcpy(service_msg.client_name,"service2");
  strcpy(service_msg.message,"this is service2");
  char *service_msg_str = new char[100];
  cout<<"\nService is going to start.\n";
  ofstream fout;
  fout.open("service2.log");
  while(true)
  {
    receive_message(sock_fd,received_msg,&service_msg_str);
    fout<<"\nMessage Received.\n";
    received_msg.print(fout);
    sock_send_message(sock_fd,service_msg);
    fout<<"\nMessage Sent.";
    service_msg.print(fout);
  }
  fout.close();
  return NULL;
}

int main()
{
  global_server_msg.service_type = 0;
  strcpy(global_server_msg.client_name,"server");

  received_str = new char[100];

  int socket_service1,socket_service2;
  socket_service1 = socket(AF_INET,SOCK_STREAM,0);
  socket_service2 = socket(AF_INET,SOCK_STREAM,0);
  sockaddr_in serv_addr;
  char *hello = "from server";
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(8080);
  socklen_t addrlen;

  bind(socket_service1,(sockaddr *)&serv_addr,sizeof(serv_addr));
  serv_addr.sin_port = htons(9090);
  bind(socket_service2,(sockaddr *)&serv_addr,sizeof(serv_addr));

  vector<int> read_sockets;
  read_sockets.push_back(socket_service1);
  read_sockets.push_back(socket_service2);

  fd_set read_fds;
  int max_fd=-1;
  listen(socket_service1,3);
  listen(socket_service2,3);
  cout<<"\nListening to all sockets.\n";
  int iteration = 1;
  while(true)
  {
    max_fd = -1;
    FD_ZERO(&read_fds);
    for(int i=0;i<read_sockets.size();i++)
    {
      FD_SET(read_sockets[i],&read_fds);
      max_fd = max(max_fd,read_sockets[i]);
    }

    int r_val = select(max_fd+1,&read_fds,NULL,NULL,NULL);

    if(r_val < 0)
    {
      cout<<"\nSelect failed.\n";
      continue;
    }

    for(int i=0;i<=max_fd;i++)
    {
      if(FD_ISSET(i,&read_fds))
      {
        int new_socket = accept(i,(sockaddr *) &serv_addr,&addrlen);
        cout<<"\nMessage received.";
        received_msg.print();
        pthread_t id;
        int* fd_ptr = new int;
        *fd_ptr = new_socket;
        if ( i== socket_service1)
        {
          // for service1
          cout<<"\nExecuting service1 thread.";
          pthread_create(&id,NULL,service1,fd_ptr);
        }
        else if( i== socket_service2)
        {
          // for service 2
          cout<<"\nExecuting service2 thread.";
          pthread_create(&id,NULL,service2,fd_ptr);
        }
        else
        {
          cout<<"\nSuch service not available now.\n";
        }
      }
    }
    cout<<"\nIteration "<<iteration++;
  }
  return 0;
}
