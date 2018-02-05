// 0 ->exiting
// 1 ->registering
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <bits/stdc++.h>
#include <sys/select.h>
#include "pipes.h"
#include "singh_message.h"

using namespace std;

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

void functional_service4(int output_fd)
{
  strcpy(server_global_message.message,"this is functinal service 4");
  //strcat(server_global_message.message," -- is received by server.");
  send_msg(output_fd);
  cout<<"\nServer: Messasge Sent.";
  //cout<<"\nMessasge sent to client -"<<received_msg.client_name<<"- ["<<clients[received_msg.client_name]<<"]";
  server_global_message.print();
  //cout<<"\nMessasge sent to client -"<<received_msg.client_name<<"- ["<<clients[received_msg.client_name]<<"]";
}

void service3(char* service_r_pipe,char* service_w_pipe)
{
  int c = fork();
  if(c==0)
  {
    int k  = open(service_r_pipe,O_RDONLY);
    cout<<"\nGoing to execute service3.";
    cout<<"\nCout will be opening "<<service_w_pipe;
    cout<<"\nCin will be opening "<<service_r_pipe<<endl;
    dup2(clients[service_w_pipe],1);
    dup2(k,0);
    char** args = NULL;
    execv("./3service",args);
  }
}

int main()
{
  received_str = new char[100];
  mkfifo(main_fifo,0666);
  mkfifo(service_fifo,0666);
  int mf_fd = open(main_fifo,O_RDONLY | O_NONBLOCK);
  int sf_fd = open(service_fifo,O_RDONLY | O_NONBLOCK);
  //cout<<"\n"<<main_fifo<<" = "<<mf_fd<<"\n"<<process_id_fifo<<" = "<<pf_fd<<endl;
  strcpy(server_global_message.client_name,"server");
  server_global_message.service_type = 0;
  fd_set read_fds,write_fds;
  int max_fd;
  read_fds_vec.push_back(mf_fd);
  read_fds_vec.push_back(sf_fd);

  cout<<"\nserver: ready for service.";
  int iteration = 0;
  while(true)
  {
    char *temp_iteration_str = new char[100];
    itoa(iteration,temp_iteration_str,10);
    strcpy(server_global_message.special_field,temp_iteration_str);

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

    // checking for all fd_set
    for(int i=0; i<max_fd+1; i++)
    {
      if(FD_ISSET(i,&read_fds))
      {
        cout<<"\nFD_ISSET->i = "<<i;
        if(receive_msg(i)<=0)
        {
          FD_CLR(i,&read_fds);
          continue;
        }
        cout<<"\nserver: Messasge received ["<<i<<"]";
        received_msg.print();
        if( i == mf_fd)
        {
          if(received_msg.service_type == 0)
          {
            // client is exiting
            read_fds_vec.erase(find(read_fds_vec.begin(),read_fds_vec.end(),clients[received_msg.client_name]));
            clients.erase(received_msg.client_name);
            cout<<"\nClient is removed.";
          }
          else if(received_msg.service_type == 1)
          {
            //registering new client
            cout<<"\nNew client -"<<received_msg.client_name<<"- is registering with pipe -"<<received_msg.read_pipe<<"-\n";
            int new_client_fd = open(received_msg.read_pipe,O_WRONLY | O_NONBLOCK );
            clients[received_msg.client_name] = new_client_fd;
            read_fds_vec.push_back(new_client_fd);
          }
          else
          {
            cout<<"\nClient -"<<received_msg.client_name<<"- asking for service -"<<received_msg.service_type<<"-.";
            switch(received_msg.service_type)
            {
              case 4:
                    // functional service1
                    functional_service4(clients[received_msg.client_name]);
                    break;
              default:
                      cout<<"\nNo such service is available at server side.\n";
                      break;
            }
          }
        }
        else if( i== sf_fd)
        {
          // someone is asking for special servises
          cout<<"\nserver: "<<received_msg.client_name<<" is asking for service "<<received_msg.service_type<<endl;
          switch(received_msg.service_type)
          {
            case 3:
                  cout<<"\nserver: Giving service3";
                  service3(received_msg.special_field,received_msg.read_pipe);
                  break;
          }
        }
        else
        {
          cout<<"\nserver: (from client's special pipe) message received from client : "<<received_msg.client_name<<endl;
          if(clients[received_msg.client_name] == 0)
          {
            cout<<"\nSuch client is not registered.So not sending any messeges.";
            continue;
          }
          switch(received_msg.service_type)
          {
            case 4:
                  // functional service1
                  functional_service4(clients[received_msg.client_name]);
                  break;
          }
        }
        FD_CLR(i,&read_fds);
      }
    }
    sleep(2);
    cout<<"\nServer: iteration "<<iteration<<".";
    iteration++;
  }

}
