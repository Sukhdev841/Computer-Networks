#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include <iostream>
#include <string.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/stat.h>

#define client1 "client1"
#define client2 "client2"

using namespace std;

int main()
{
	int c1,c2;
	c1 = open(client1,O_RDONLY);
	c2 = open(client2,O_RDONLY);
	cout<<"\nserver opened two clients "<<c1<< " "<<c2<<endl;
	pollfd fds[]={{c1,POLLIN},{c2,POLLIN}};
	cout<<"\nAfter declaring pool ids\n";
	int n = 2;

	// now doing poll
	cout<<"\ndoing poll\n";
	while(true)
	{
		int r = poll(fds,n,3500);
		if(r<0)
		{
			cout<<"\nError occur in reading poll\n";
		}
		if(r==0)
		{
			cout<<"\nTimeout occured!\n";
		}
		else
		{
			cout<<"\nin else block\n";
			for(int i=0;i<n;i++)
			{
				if(fds[i].revents & POLLIN)
				{
					// this event occur
					char buff[100];
					read(fds[i].fd,buff,99);
					cout<<endl<<"Read from i = "<<i<<" : "<<buff<<endl;
					//clear(fds[i].fd);
				}
			}
		}
	}
		
	return 0;
}