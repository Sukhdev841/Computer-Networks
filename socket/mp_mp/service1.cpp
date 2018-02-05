#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include <arpa/inet.h>
#include <linux/socket.h>
#include <unistd.h>
#include <bits/stdc++.h>
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
  ofstream fout;
  fout.open("service1.log");
  while(true)
  {
    receive_message(0,received_msg,&service_msg_str);
    fout<<"\nMessage Received.\n";
    received_msg.print(fout);
    sock_send_message(0,service_msg);
    fout<<"\nMessage Sent.";
    service_msg.print(fout);
  }
}
