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
  cout<<"\nIn service1\n";
  message_t service_msg,received_msg;
  strcpy(service_msg.client_name,"service1");
  strcpy(service_msg.message,"this is service1");
  char *service_msg_str = new char[100];
  int sock_fd ;
  cout<<"\nService is going to start.\n";
  while(true)
  {
    receive_message(0,received_msg,&service_msg_str);
    cout<<"\nMessage Received.\n";
    received_msg.print();
    sock_send_message(0,service_msg);
    cout<<"\nMessage Sent.";
  }
}
