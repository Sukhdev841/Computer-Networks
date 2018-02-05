#include <stdio.h>
#include <unistd.h>
#include <poll.h>
#include <iostream>
#include <string.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <fcntl.h>

using namespace std;

#define main_pipe "main_pipe"

int main(int argc, char *argv[]) 
{
    char *msg = new char[100];
    cout<<"\nEnter name of client : ";
    cin>>msg;

    cout<<"\nName input taken\n";

    cout<<"\nSending name to server\n";

    int wfd = open(main_pipe,O_WRONLY);

    write(wfd,msg,strlen(msg)+1);

    cout<<"\nwritten to server : "<<msg<<endl;

    close(wfd);

    cout<<"\nmain pipe closed\n";

    wfd = open(msg,O_WRONLY);

    cout<<"\nnew pipe opened : ";

    while(true)
    {
        cout<<"\nEnter message : ";
        cin>>msg;
        write(wfd,msg,strlen(msg)+1);
        cout<<"\nsent to server\n";
    }

    return 0;
}