#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <bits/stdc++.h>
#include <signal.h>
#include <string.h>
#include <cctype>
#include <cstring>

using namespace std;


void handler(int sig)
{
	//signal(sig,SIG_IGN);
	signal(SIGINT,handler);
	cout<<"\ngood\n";
	
}

int main()
{
	signal(SIGINT,handler);
	while(1)
	{
		sleep(1);
	}

	return 0;

}