// 0 ->exiting
// 1 ->registering
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <bits/stdc++.h>
#include <sys/select.h>
#include <signal.h>
#include "pipes.h"
#include "singh_message.h"

using namespace std;

int main()
{
    cout<<"\nIn special process\n";
    int fd = open(process_4_fd,O_RDONLY);
    char str[100];
    cout<<"\nNow reading from fd "<<fd<<endl;
    read(fd,str,100);
    cout<<"\nRead data is : "<<str<<endl;
    close(fd);
    return 0;
}
