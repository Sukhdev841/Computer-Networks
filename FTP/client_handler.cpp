// 0 ->exiting
// 1 ->registering
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <bits/stdc++.h>
#include <sys/select.h>
#include <signal.h>
#include "singh_message.h"
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

#define PORT 8021

using namespace std;

int server_pid = -1;
char client_main_read_pipe[100];
int client_r_fd=-1,client_w_fd=-1,server_r_fd = -1,server_service_fd = -1;
message_t client_global_message;
char name[100];   // client's write pipe (for service 3 only)

int main()
{
    cout<<"\nRegistering for communication.";
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }

    cout<<"\nClient Successfully connected to servre.\n";

    char *received_str = new char[100];
    char *temp = new char[100];
    message_t received_msg;

    while(true)
    {
      int type;
      cout<<"\n1.Query.\n2.File Download.\n";
      cin>>type;
      cin.clear();
      cin.ignore();
      int s_type;
      switch(type)
      {
        case 1:
                client_global_message.service_type = 1;
                client_global_message.print();
             send_message(sock,client_global_message);
                //write(sock,"hello",strlen("hello"+1));
              cout<<"\nMessage sent by client.";
              cout<<"\nMessage sent by client.";
               receive_message(sock,received_msg,&received_str);
               cout<<"\nMessage received by client. == ";
               received_msg.print();
              break;
        case 2:
              client_global_message.service_type = 2;
              strcpy(client_global_message.special_field,name);
              strcpy(client_global_message.read_pipe,client_main_read_pipe);
              send_message(sock,client_global_message);
              cout<<"\nMessage sent for service 3\n";
              client_global_message.print();
              cout<<"\nClient will be wrting to "<<name<<endl;
              cout<<"\nClient will be reading from "<<client_main_read_pipe<<endl;
              while(true)
              {
                // now communicating with service 3 only
                // read from client_r_fd and write to client_w_fd
                receive_message(sock,received_msg,&received_str);
                cout<<"\nMessage Received from service3.";
                received_msg.print();
                cout<<"\nEnter 1 if you wish to continue : ";
                cin>>type;
                cin.clear();
                cin.ignore();
                if(type != 1)
                  break;
                cout<<"\nEnter message to send.";
                cin>>client_global_message.message;
                send_message(sock,client_global_message);
                cout<<"\nSent message\n";
                client_global_message.print();
                sleep(1);
              }
              break;
      }
    }
}
