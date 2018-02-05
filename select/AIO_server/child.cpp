#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <bits/stdc++.h>
#include <sys/select.h>
#include "pipes.h"
#include "singh_message.h"

using namespace std;
message_t child_global_message;
int main()
{
  char *rec_msg_str = new char[100];

  ofstream fout;
  fout.open("child_log.txt");
  char temp[100];
  const char* enter_message = "\nEnter message to be sent to server: ";
  const char* mess_rec = "\nMessagse is received.";
  const char* mess_sent = "\nMessage sent: ";
  const char* my_msg = "hello_server";
  const char* client_name = "child";
  strcpy(child_global_message.message,my_msg);
  while(true)
  {
    child_global_message.service_type = 2;
    strcpy(child_global_message.client_name,client_name);
    strcpy(child_global_message.message,my_msg);
    send_message(1,child_global_message);
    fout<<"\nMessage Sent\n";
    child_global_message.print(fout);
    receive_message(0,child_global_message,&rec_msg_str);
    child_global_message.print(fout);
    sleep(7);
  }
  return 0;
}
