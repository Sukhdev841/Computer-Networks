#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <iostream>
#include "singh_message.h"

using namespace std;

message_t received_msg;
char *received_str;
message_t global_server_msg;

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
    int new_sock = accept(serv_socket,(sockaddr *) &serv_addr,&addrlen);
    int c = fork();
    if( c == 0)
    {
      cout<<"\nA client Came.";
      receive_message(new_sock,received_msg,&received_str);
      char *service_ =new char[100];
      strcpy(service_,"./");
      strcat(service_,received_msg.message);
      cout<<"\nService -"<<service_<<"- is going to executed.";
      dup2(new_sock,0);
      cout<<"\ndup2 done.\n";
      cout<<"\ndup2 done.\n";
      char **args = NULL;
      execv("./service1",args);
      exit(0);
    }
  }
  return 0;
}
