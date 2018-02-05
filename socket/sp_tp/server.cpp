#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <pthread.h>
#include "singh_message.h"

using namespace std;

message_t received_msg;
char *received_str;
message_t global_server_msg;

void *service1(void *args)
{
  int *sock_fd = (int*)(args);
  cout<<"\nIn service1\n";
  message_t service_msg,received_msg;
  strcpy(service_msg.client_name,"service1");
  strcpy(service_msg.message,"this is service1");
  char *service_msg_str = new char[100];
  cout<<"\nService is going to start.\n";
  while(true)
  {
    receive_message(*sock_fd,received_msg,&service_msg_str);
    cout<<"\nMessage Received.\n";
    received_msg.print();
    sock_send_message(*sock_fd,service_msg);
    cout<<"\nMessage Sent.";
  }
  return NULL;
}

int main()
{
  global_server_msg.service_type = 0;
  strcpy(global_server_msg.client_name,"server");

  received_str = new char[100];

  int serv_socket;
  serv_socket = socket(AF_INET,SOCK_STREAM,0);
  sockaddr_in serv_addr;
  char *hello = "from server";
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(8080);
  socklen_t addrlen;

  bind(serv_socket,(sockaddr *)&serv_addr,sizeof(serv_addr));
  while(true)
  {
    listen(serv_socket,3);
    cout<<"\nClient came.";
    int new_sock = accept(serv_socket,(sockaddr *) &serv_addr,&addrlen);
    receive_message(new_sock,received_msg,&received_str);
    int *p = new int;
    *p = new_sock;
    pthread_t id;
    switch(received_msg.service_type)
    {
      case 1:
            cout<<"\nClient requesting service : "<<received_msg.service_type;
            pthread_create(&id,NULL,service1,p);
            break;
    }
  }
  return 0;
}
