// 0 ->exiting
// 1 ->registering
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <bits/stdc++.h>
#include <sys/select.h>
#include <signal.h>
#include "pipes.h"
#include "singh_message.h"

using namespace std;

int server_pid = -1;
char client_main_read_pipe[100];
int client_r_fd=-1,client_w_fd=-1,server_r_fd = -1,server_service_fd = -1;
message_t client_global_message;
char name[100];   // client's write pipe (for service 3 only)

int main()
{
    cout<<"\nClient pid = "<<getpid();
    cout<<"\nEnter client's pipe name : ";
    cin>>client_main_read_pipe;
    if(mkfifo(client_main_read_pipe,0666)<0)
    {
      cout<<"\nError :(\n mkfifo failed with pipe name "<<client_main_read_pipe<<endl;
      exit(1);
    }
    cout<<"\nEnter name of pipe to which service3 will be reading : ";
    cin>>name;
    if(mkfifo(name,0666)<0)
    {
      cout<<"\nmkfifo failed with pipe name "<<name<<endl;
      exit(0);
    }
    strcpy(client_global_message.client_name,client_main_read_pipe);
    strcpy(client_global_message.read_pipe,client_main_read_pipe);

    char *received_str = new char[100];
    char *temp = new char[100];
    message_t received_msg;

    client_r_fd = open(client_main_read_pipe,O_RDONLY|O_NONBLOCK);
    client_w_fd = open(name,O_RDWR|O_NONBLOCK);
    server_r_fd = open(main_fifo,O_WRONLY|O_NONBLOCK);
    server_service_fd = open(service_fifo,O_WRONLY|O_NONBLOCK);
    cout<<"\nclient_r_fd : "<<client_r_fd<<"\nclient_w_fd : "<<client_w_fd<<"\nserver_r_fd : "<<server_r_fd<<endl;
    if(client_r_fd < 1)
    {
      cout<<"\nError :(\n open failed.\n";
      exit(1);
    }
    // register client
    cout<<"\nRegistering for communication.";
    client_global_message.service_type = 1;   // 1 for registering
    send_message(server_r_fd,client_global_message);
    cout<<"\nClient is ready to communicate with server :)";
    int r,temp_fd;
    while(true)
    {
      int type;
      cout<<"\n1.Functional Service.\n2.Process service3.\n3.Exit\n";
      cin>>type;
      cin.clear();
      cin.ignore();
      int s_type;
      switch(type)
      {
        case 1:
              cout<<"\nEnter service number : ";
              cin>>client_global_message.service_type;
              cout<<"\nEnter message : ";
              cin>>client_global_message.message;
              send_message(server_r_fd,client_global_message);
              cout<<"\nMessage sent by client.";
              receive_message(client_r_fd,received_msg,&received_str);
              cout<<"\nMessage received by client. == ";
              received_msg.print();
              break;
        case 2:
              client_global_message.service_type = 3;
              strcpy(client_global_message.special_field,name);
              strcpy(client_global_message.read_pipe,client_main_read_pipe);
              send_message(server_service_fd,client_global_message);
              cout<<"\nMessage sent for service 3\n";
              client_global_message.print();
              cout<<"\nClient will be wrting to "<<name<<endl;
              cout<<"\nClient will be reading from "<<client_main_read_pipe<<endl;
              while(true)
              {
                // now communicating with service 3 only
                // read from client_r_fd and write to client_w_fd
                receive_message(client_r_fd,received_msg,&received_str);
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
                send_message(client_w_fd,client_global_message);
                cout<<"\nSent message\n";
                client_global_message.print();
                sleep(1);
              }
              break;
        case 3:
              client_global_message.service_type = 0;
              send_message(server_r_fd,client_global_message);
              cout<<"\nMessagse for closing client fd is sent to server.\n";
              close(client_r_fd);
              exit(0);
              break;
      }
    }
}
