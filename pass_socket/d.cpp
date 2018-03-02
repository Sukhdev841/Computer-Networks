#include <bits/stdc++.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std;

int main()
{
  char buff[100];
  memset(&buff,'0',strlen(buff));
  cout<<"\nReading\n";
  cin>>buff;
  cout<<"\nRead by D : "<<buff<<endl;

  return 0;

}
