#include "ace/SOCK_Acceptor.h"
#include "ace/SOCK_Stream.h"
#include "ace/Log_Msg.h"

class Server
{
private:
	char *data_buff_;
	ACE_INET_Addr server_addr_;
	ACE_INET_Addr client_addr_;
	ACE_SOCK_Acceptor peer_acceptor_;
	ACE_SOCK_Stream new_stream_;

public:
	Server(int port):
		server_addr_(port),peer_acceptor_(server_addr_)
	
	{
		data_buff_ = new char[1024];
	}

	int accept_connection()
	{
		if(peer_acceptor_.get_local_addr(server_addr_) == -1)
			ACE_ERROR_RETURN((LM_ERROR,"%p\n","Error in get_local_addr"),1);
		ACE_DEBUG((LM_DEBUG,"Starting server at port %d\n",server_addr_.get_port_number()));

		while(1)
		{
			if(peer_acceptor_.accept(new_stream_,&client_addr_,NULL) == -1)
			{
				ACE_ERROR((LM_ERROR,"%p\n","accept"));
				continue;
			}
			else
			{
				ACE_DEBUG((LM_DEBUG,"Connection established with %s:%d\n",client_addr_.get_host_name(),client_addr_.get_port_number()));
				// handling connection
				handle_connection();
			}
		}
	}

	int handle_connection()
	{
		//Receiving msg from client
		int byte_count_ = 0;
		if((byte_count_ = new_stream_.recv_n(data_buff_,1024,10)) == -1)
			ACE_ERROR((LM_ERROR,"%p\n","Error receiving msg"));
		else
		{
			ACE_DEBUG((LM_DEBUG,"byte_count = %d\n",byte_count_));
			ACE_DEBUG((LM_DEBUG,"[Server] Message received : %s\n",data_buff_));
			new_stream_.send_n("Hello from Server",ACE_OS::strlen("Hello from Server")+1,0);
		}

		// close the stream
		if(new_stream_.close() == -1)
			ACE_ERROR((LM_ERROR,"%p\n","closing stream error"));
		return 0;
	}
};

int main(int argc,char* argv[])
{
	if(argc < 2)
	{
		ACE_ERROR((LM_ERROR,"Usage is %s <port_num>\n",argv[0]));
		ACE_OS::exit(1);
	}
	Server server(ACE_OS::atoi(argv[1]));
	server.accept_connection();
}