#include <unistd.h>
#include <iostream>
#include <sys/types.h>
#include <fcntl.h>

using namespace std;

int main()
{
	int fd = open("readme.txt",O_RDONLY);
	char str[50];
	read(fd,str,6);
	cout<<"\nRead from file by fd : "<<str<<endl;
	close(0);
	fcntl(fd,F_DUPFD,0);
	read(0,str,6);
	cout<<"\nRead from file by 0: "<<str<<endl;
	return 0;
}