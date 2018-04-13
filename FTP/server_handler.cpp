// 0 ->exiting
// 1 ->registering
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <bits/stdc++.h>
#include <sys/select.h>
#include "singh_message.h"
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>

using namespace std;

#define PORT 8021

map<string,int> clients;  // <client_name,client_fd>
vector<int> read_fds_vec;
vector<int> write_fds_vec;
message_t server_global_message;
char *received_str;
message_t received_msg;

int send_msg(int fd)
{
  return send_message(fd,server_global_message);
}

int receive_msg(int fd)
{
  return receive_message(fd,received_msg,&received_str);
}

void send_file_list(int sfd)
{
  strcpy(server_global_message.message,"ok");
  send_msg(sfd);
  cout<<"\nReply sent to client\n";
}

int main()
{
  int server_fd, new_socket, valread;
  struct sockaddr_in address;
  int opt = 1;
  int addrlen = sizeof(address);
  char buffer[1024] = {0};
  char *hello = "Hello from server";

  // Creating socket file descriptor
  if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
  {
      perror("socket failed");
      exit(EXIT_FAILURE);
  }

  // Forcefully attaching socket to the port 8080
  if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,

                                         &opt, sizeof(opt)))
  {
      perror("setsockopt");
      exit(EXIT_FAILURE);
  }
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = INADDR_ANY;
  address.sin_port = htons( PORT );

  // Forcefully attaching socket to the port 8080
  if (bind(server_fd, (struct sockaddr *)&address,
                               sizeof(address))<0)
  {
      perror("bind failed");
      exit(EXIT_FAILURE);
  }
  if (listen(server_fd, 3) < 0)
  {
      perror("listen");
      exit(EXIT_FAILURE);
  }


  // main socket done

  received_str = new char[100];
  
  strcpy(server_global_message.client_name,"server");
  server_global_message.service_type = 0;
  fd_set read_fds;
  int max_fd;
  read_fds_vec.push_back(server_fd);

  cout<<"\nserver: ready for service.";
  int iteration = 0;
  while(true)
  {

    FD_ZERO(&read_fds);
    //FD_ZERO(&write_fds);
    max_fd = -1;

    for(int i=0;i<read_fds_vec.size();i++)
    {
        cout<<"\nRegistering : "<<read_fds_vec[i];
        FD_SET(read_fds_vec[i],&read_fds);
        max_fd = max(read_fds_vec[i],max_fd);
    }
    // for(int i=0;i<write_fds_vec.size();i++)
    // {
    //   FD_SET(write_fds_vec[i],&write_fds);
    //   max_fd = max(write_fds_vec[i],max_fd);
    // }
    cout<<"\nmax_fd = "<<max_fd;
    int ret_val = select(max_fd+1,&read_fds,NULL,NULL,NULL);

    if(ret_val == 0)
    {
        cout<<"\nserver: timeout.";
        sleep(2);
        continue;
    }
    if(ret_val < 1)
    {
        cout<<"\nserver: select failed."<<ret_val;
        sleep(2);
        continue;
    }

    if(FD_ISSET(read_fds_vec[0],&read_fds))
    {
      // a new client wants to connect
      int new_client;
      if ((new_client = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
      {
          perror("accepting new client error");
          //exit(EXIT_FAILURE);
      }
      read_fds_vec.push_back(new_client);
      cout<<"\nNew client added to list\n";
    }

    // checking for all fd_set
    for(int i=1; i<read_fds_vec.size(); i++)
    {
      if(FD_ISSET(read_fds_vec[i],&read_fds))
      {
        cout<<"\nFD_ISSET->i = "<<read_fds_vec[i];
        cout<<"\nFD_ISSET->i = "<<read_fds_vec[i];
        if(receive_msg(read_fds_vec[i])<=0)
        {
          FD_CLR(read_fds_vec[i],&read_fds);
          continue;
        }
        cout<<"\nserver: Messasge received ["<<read_fds_vec[i]<<"]";
        sockaddr_in temp_addr;
        int length = sizeof(temp_addr);
        int ret = getpeername(read_fds_vec[i],(sockaddr*)&temp_addr,(socklen_t*)&length);
        printf("\nPeer's address is : %s\n",inet_ntoa(temp_addr.sin_addr));
        printf("\nPeer's port number is : %d\n",(int) ntohs(temp_addr.sin_port));
        received_msg.print();

        // check type of service
        switch(received_msg.service_type)
        {
          case 1:
                  send_file_list(read_fds_vec[i]);
                  break;
          case 2:
                  cout<<"\nServer client wants server to send file\n";
                  break;
          default:
                  cout<<"\nServer: error undefined service number\n";
        }

        FD_CLR(read_fds_vec[i],&read_fds);
      }
    }
    sleep(2);
    cout<<"\nServer: iteration "<<iteration<<".";
    iteration++;
  }

}
