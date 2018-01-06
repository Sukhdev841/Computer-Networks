#include <unistd.h>
#include <iostream>

using namespace std;

int main()
{
	int fd = fileno(popen("./p2","r"));
	char str[30];
	read(fd,str,10);
	cout<<"\nRead by p1 : "<<str<<endl;

	// int fd = fileno(popen("./p2","r"));
	// write(fd,"input from program p1",50);
	
	return 0;
}