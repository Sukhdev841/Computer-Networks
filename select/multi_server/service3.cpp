#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <bits/stdc++.h>
#include <sys/select.h>
#include "pipes.h"
#include "singh_message.h"

using namespace std;
message_t service_global_message;
message_t received_msg;
char* received_str;
int main()
{
  char *rec_msg_str = new char[100];
  received_str = new char[100];

  ofstream fout;
  fout.open("log3.txt");
  char temp[100];
  strcpy(service_global_message.client_name,"service3");
  strcpy(service_global_message.message,"this is service3");
  service_global_message.service_type = 1;
  while(true)
  {
    send_message(1,service_global_message);
    fout<<"\nMessage Sent\n";
    service_global_message.print(fout);
    receive_message(0,received_msg,&received_str);
    fout<<"\nMessage Received.";
    received_msg.print(fout);
    sleep(3);
  }
  return 0;
}
