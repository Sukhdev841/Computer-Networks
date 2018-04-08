// Server side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <bits/stdc++.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <fcntl.h>

using namespace std;

#define global_fifo "my_fifo"

int main(int argc, char const *argv[])
{
    mkfifo(global_fifo,0666);
    // int c = fork();
    // if(c == 0)
    // {
    //     char **x = NULL;
    //     execv("sudo ./tracker",x);
    //     exit(0);
    // }
    int fd = open(global_fifo,O_RDWR);
    cout<<"\nfd = "<<fd<<endl;

    fd_set read_fds;
    fd_set write_fds;
    
    vector<int> fds;
    vector<int> w_fds;
    vector<int> listen_fds;
    fds.push_back(fd);
    int max_ = fd;

    struct sockaddr_in addr;
    int len = sizeof(addr);

    while(true)
    {
        char buff[1024];
        cout<<endl<<"[Read] : ";
        for(int i=0;i<fds.size();i++)
        {
            cout<<fds[i]<<" ";
            FD_SET(fds[i],&read_fds);
            max_ = max(max_,fds[i]);
        }
        cout<<endl<<"[Listen] : ";
        for(int i=0;i<listen_fds.size();i++)
        {
            cout<<listen_fds[i]<<" ";
            FD_SET(listen_fds[i],&read_fds);
            max_ = max(max_,listen_fds[i]);
        }
        cout<<endl<<"[Write] : ";
        for(int i=0;i<w_fds.size();i++)
        {
            cout<<w_fds[i]<<" ";
            FD_SET(w_fds[i],&write_fds);
            max_ = max(max_,w_fds[i]);
        }
        cout<<"\nBefore select max = "<<max_<<endl;
        if( select(max_+1,&read_fds,&write_fds,NULL,NULL) < 0)
        {
            printf("\nSelect failed\n");
            sleep(2);
            continue;
        }

        if(FD_ISSET(fd,&read_fds))
        {
            printf("\n+-----------------------------------------------------------------------+\n");
            read(fd,buff,sizeof(buff));
            int new_port = atoi(buff);
            printf("\nBinding to port %d\n",new_port);
            int new_sock = socket(AF_INET,SOCK_STREAM,0);
            struct sockaddr_in addr;
            addr.sin_family = AF_INET;
            addr.sin_addr.s_addr = inet_addr("172.30.100.178");
            addr.sin_port = htons(new_port);
            while(bind(new_sock,(struct sockaddr*)&addr,sizeof(addr)) < 0)
            {
                printf("Bind failed\n");
                sleep(1);
            }
            while(listen(new_sock,10) < 0)
            {
                printf("listen failed\n");
                sleep(1);
            }
            listen_fds.push_back(new_sock);
            printf("\nNew socket bind to port : %d and new socket is %d\n",new_port,new_sock);
            FD_CLR(fd,&read_fds);
            printf("\n+-----------------------------------------------------------------------+\n");
        }

        for(int i=0;i<listen_fds.size();i++)
        {
            if(FD_ISSET(listen_fds[i],&read_fds))
            {
                printf("\n+-----------------------------------------------------------------------+\n");
                printf("\n[Listening] Accepting new client on fd : %d\n",listen_fds[i]);
                int new_client = accept(listen_fds[i],(struct sockaddr*)&addr,(socklen_t*)&len);
                printf("\n[Listen] new client is : %d\n",new_client);
                printf("\n%s : %d\n",inet_ntoa(addr.sin_addr),(int) ntohs(addr.sin_port));
                fds.push_back(new_client);
                w_fds.push_back(new_client);
                printf("\n+-----------------------------------------------------------------------+\n");
            }
        }

        for(int i=1;i<fds.size();i++)
        {
            if(FD_ISSET(fds[i],&read_fds))
            {
                printf("\n+-----------------------------------------------------------------------+\n");
                printf("\n[Receving] from fd : %d\n",fds[i]);
                getpeername(fds[i],(sockaddr*)&addr,(socklen_t*)&len);
                printf("\n%s : %d\n",inet_ntoa(addr.sin_addr),(int) ntohs(addr.sin_port));
                char msg[1024];
                memset(msg,0,1024);
                if(read(fds[i],msg,sizeof(msg))<= 0)
                {
                    printf("\nClient is fucked up while reading. closing it\n");
                    close(w_fds[i]);
                    w_fds.erase(find(w_fds.begin(),w_fds.end(),fds[i]));
                    fds.erase(find(fds.begin(),fds.end(),fds[i]));
                    continue;
                }
                printf("\nMsg is : %s\n",msg);
                printf("\n+-----------------------------------------------------------------------+\n");
                
            }
        }

        for(int i=0;i<w_fds.size();i++)
        {
            if(FD_ISSET(w_fds[i],&write_fds))
            {
                printf("\n+-----------------------------------------------------------------------+\n");
                printf("\n[Writing] fd : %d\n",w_fds[i]);
                getpeername(w_fds[i],(sockaddr*)&addr,(socklen_t*)&len);
                printf("\n%s : %d\n",inet_ntoa(addr.sin_addr),(int) ntohs(addr.sin_port));
                if(write(w_fds[i],"Hello from symbhu server.",strlen("Hello from symbhu server.")+1) <= 0)
                {
                    printf("\nClient is fucked up while writing closing it\n");
                    close(w_fds[i]);
                    fds.erase(find(fds.begin(),fds.end(),w_fds[i]));
                    w_fds.erase(find(w_fds.begin(),w_fds.end(),w_fds[i]));
                }
                printf("\n+-----------------------------------------------------------------------+\n");
            }
        }
        sleep(2);
    }
    return 0;
}
