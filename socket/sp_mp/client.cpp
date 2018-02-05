#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <arpa/inet.h>
#include <linux/socket.h>
#include <unistd.h>
#include "singh_message.h"

using namespace std;

int main()
{
  sockaddr_in serv_addr;
  int client_sock = socket(AF_INET,SOCK_STREAM,0);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(8080);
  inet_pton(AF_INET,"127.0.0.1",&serv_addr.sin_addr);
  connect(client_sock,(sockaddr *)&serv_addr,sizeof(serv_addr));
  message_t client_msg;
  char *client_msg_str = new char[100];
  cout<<"\nSelect a service.\n";
  cout<<"\n1.service1\n";
  int type;
  cin>>type;
  switch(type)
  {
    case 1:
          strcpy(client_msg.message,"service1");
          sock_send_message(client_sock,client_msg);
          break;
  }
  cout<<"\nMessasge is sent to server.";
  strcpy(client_msg.client_name,"client");
  while(true)
  {
    cout<<"\nEnter 0 to exit: ";
    cin>>type;
    cin.clear();
    cin.ignore();
    if(type == 0)
      break;
    cout<<"\nType message : ";
    cin>>client_msg.message;
    sock_send_message(client_sock,client_msg);
    cout<<"\nMessage sent to the service.";
    receive_message(client_sock,received_msg,&client_msg_str);
    cout<<"\nMessage received.\n";
    received_msg.print();
  }
  return 0;
}
