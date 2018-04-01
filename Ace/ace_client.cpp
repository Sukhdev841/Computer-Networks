#include "ace/SOCK_Connector.h"
#include "ace/Log_Msg.h"
#include <bits/stdc++.h>

using namespace std;

static u_short SERVER_PORT = 8080;
static char* SERVER_HOST = "127.0.0.1";

int main(int argc,char* argv[])
{
	if(argc < 2)
		ACE_ERROR_RETURN((LM_ERROR,"Usage : %s <server-port>\n",argv[0]),1);
	ACE_SOCK_Stream server;
	ACE_SOCK_Connector connector;
	ACE_INET_Addr addr(ACE_OS::atoi(argv[1]),SERVER_HOST);
	if(connector.connect(server,addr) == -1)
		cout<<"\nConnector error\n";
	char *msg = "Hello from client\n\0";
	char buffer[1024];
	server.send_n(msg,ACE_OS::strlen(msg)+1,0);
	cout<<"\nMsg sent\n";
	server.recv_n(buffer,1024,0);
	ACE_DEBUG((LM_DEBUG,"[Client] Message received : %s\n",buffer));
	server.close();
	return 0;
}