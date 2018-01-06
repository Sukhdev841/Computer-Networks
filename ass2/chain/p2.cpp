#include <unistd.h>
#include <bits/stdc++.h>
#include <sys/types.h>
#include <fcntl.h>


using namespace std;

int main()
{
	string str;
	//fflush(stdout);
	//cout<<"\np2: enter input : ";
	cin>>str;
	cout<<"\ninput of p2  : "<<str<<endl;

	int fdin,fdout;
	fdin = open("temp.txt",O_RDONLY);
	fdout = open("temp.txt",O_WRONLY);
	int c = fork();
	

	if(c > 0)
	{
		dup2(fdout,1);
		cout<<"output_of_p2\n";
	}
	else
	{
		dup2(fdin,0);
		char **t = NULL;
		execv("./p3",t);
	}

	return 0;
	exit(0);

}