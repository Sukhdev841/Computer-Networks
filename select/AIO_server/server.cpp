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

int p4fd = -1;

char *global_data = "this is global data";

void signal_handler(int sig)
{
  signal(SIGUSR1,signal_handler);

  cout<<"\n################################################\n";
  cout<<"\n################################################\n";
  cout<<"\n################################################\n";
  cout<<"\nSignal is received by server.\n";
  cout<<"\n################################################\n";
  cout<<"\n################################################\n";
  cout<<"\n################################################\n";
  if(p4fd == -1)
  {
    cout<<"\np4fd was -1\n";
    p4fd = open(process_4_fd,O_WRONLY);
    cout<<"\nnow p4fd = "<<p4fd;
  }
  write(p4fd,global_data,strlen(global_data)+1);
  cout<<"\nData is sent to process4\n";
}

int send_msg(int fd)
{
  return send_message(fd,server_global_message);
}

int receive_msg(int fd)
{
  return receive_message(fd,received_msg,&received_str);
}

void service3(char* client_name,char* client_write_pipe)
{
  int c = fork();
  if(c==0)
  {
    cout<<"\nGoing to execute service3.";
    dup2(clients[client_name],1);
    dup2(open(client_write_pipe,O_RDONLY),0);
    char** args = NULL;
    execv("./3service",args);
  }
}

void child(int r_fd,int w_fd)
{
  cout<<"\nIn child\n";
  cout<<"\nIn child\n";
  cout<<"\nr_fd "<<r_fd<<endl;
  cout<<"\nw_fd "<<w_fd<<endl;
  dup2(r_fd,STDIN_FILENO);
  dup2(w_fd,STDOUT_FILENO);
  close(r_fd);
  close(w_fd);
  cout<<"0|child|Child execution done|||\n";
  char **args = NULL;
   //char * const args[]={"/bin/gnome-terminal","-x","./child",(char*)0};
  execv("./child",args);
}

int main()
{
  signal(SIGUSR1,signal_handler);
  mkfifo(process_4_fd,0777);
  received_str = new char[100];
  mkfifo(main_fifo,0666);
  mkfifo(process_id_fifo,0666);
  int mf_fd = open(main_fifo,O_RDONLY | O_NONBLOCK);
  int pf_fd = open(process_id_fifo,O_RDONLY | O_NONBLOCK);
  cout<<"\n"<<main_fifo<<" = "<<mf_fd<<"\n"<<process_id_fifo<<" = "<<pf_fd<<endl;
  strcpy(server_global_message.client_name,"server");
  server_global_message.service_type = 0;
  fd_set read_fds,write_fds;
  int max_fd;
  read_fds_vec.push_back(mf_fd);
  read_fds_vec.push_back(pf_fd);
  int ch_fd1[2],ch_fd2[2];
  if(pipe(ch_fd1)<0 | pipe(ch_fd2)<0)
  {
    cout<<"\npipe failed\n";
    exit(0);
  }
  // server reads from ch_fd1[0]
  // server writes to ch_fd2[1]
  int c = fork();
  if(c==0)
  {
    close(ch_fd1[0]);
    close(ch_fd2[1]);
    child(ch_fd2[0],ch_fd1[1]);
    exit(0);
  }
  read_fds_vec.push_back(ch_fd1[0]);
  clients["child"]=ch_fd2[1];
  cout<<"\nWrite fd of child = "<<clients["child"]<<" but acutal "<<ch_fd2[1]<<endl;
  close(ch_fd1[1]);
  close(ch_fd2[0]);
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
            int new_client_fd = open(received_msg.read_pipe,O_WRONLY | O_NONBLOCK );
            clients[received_msg.client_name] = new_client_fd;
            read_fds_vec.push_back(new_client_fd);
          }
          else if(received_msg.service_type == 2)
          {
            // a registered client has sent message reply back
            cout<<"\nserver: (service type 2) message received from client : "<<received_msg.client_name<<endl;
            if(clients[received_msg.client_name] == 0)
            {
              cout<<"\nSuch client is not registered.So not sending any messeges.";
              continue;
            }
            strcpy(server_global_message.message,received_msg.message);
            strcat(server_global_message.message," -- is received by server.");
            send_msg(clients[received_msg.client_name]);
            cout<<"\nServer: Messasge Sent.";
            //cout<<"\nMessasge sent to client -"<<received_msg.client_name<<"- ["<<clients[received_msg.client_name]<<"]";
            server_global_message.print();
          }
          else if(received_msg.service_type == 3)
          {
            // client is requesting for service 3
            service3(received_msg.client_name,received_msg.special_field);
          }
          else
          {
            cout<<"\nInvalid service requested by client.";
          }
        }
        else if( i== pf_fd)
        {
          // someone is asking process id of server
          char *temp = new char[100];
          cout<<"\nserver: pid = "<<getpid();
          itoa(getpid(),temp,10);
          cout<<"\nServer sending : -"<<server_global_message.special_field<<"-";
          strcpy(server_global_message.special_field,temp);
          send_msg(clients[received_msg.client_name]);
          cout<<"\nProcess id of server is sent to client "<<received_msg.client_name<<" ["<<received_msg.special_field<<"]";
        }
        else
        {
          cout<<"\nserver: (from client's special pipe) message received from client : "<<received_msg.client_name<<endl;
          if(clients[received_msg.client_name] == 0)
          {
            cout<<"\nSuch client is not registered.So not sending any messeges.";
            continue;
          }
          strcpy(server_global_message.message,received_msg.message);
          //strcat(server_global_message.message," -- is received by server.");
          send_msg(clients[received_msg.client_name]);
          cout<<"\nServer: Messasge Sent.";
          //cout<<"\nMessasge sent to client -"<<received_msg.client_name<<"- ["<<clients[received_msg.client_name]<<"]";
          server_global_message.print();
          //cout<<"\nMessasge sent to client -"<<received_msg.client_name<<"- ["<<clients[received_msg.client_name]<<"]";
        }
        FD_CLR(i,&read_fds);
      }
    }
    sleep(2);
    cout<<"\nServer: iteration "<<iteration<<".";
    iteration++;
  }

}
