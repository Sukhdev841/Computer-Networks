// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#define PORT 8080
#define BUFLEN 512

void die(char *s)
{
    perror(s);
    exit(1);
}

int main(int argc, char const *argv[])
{
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Convert IPv4 and IPv6 addresses from text to binary form
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    // send(sock , hello , strlen(hello) , 0 );
    // printf("Hello message sent\n");
    // valread = read( sock , buffer, 1024);
    // printf("%s\n",buffer );
    close(sock);


    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    ///////////////////////////////////////////////////////////////////////////////////////////////////////






    struct sockaddr_in si_other;
    int s, i, slen=sizeof(si_other);
    char buf[BUFLEN];
    char message[BUFLEN];

    if ( (s=socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
    {
        die("socket");
    }

    memset((char *) &si_other, 0, sizeof(si_other));
    si_other.sin_family = AF_INET;
    si_other.sin_port = htons(50037);

    if (inet_aton("127.0.0.1" , &si_other.sin_addr) == 0)
    {
        fprintf(stderr, "inet_aton() failed\n");
        exit(1);
    }

    bind(s , (struct sockaddr*)&si_other, sizeof(si_other) );

    socklen_t len;

    if (recvfrom(s, buf, BUFLEN, 0, NULL, 0 ) == -1)
    {
        die("recvfrom()");
    }

    // int length;
    // sockaddr_in temp_addr;
    //
    // int ret = getpeername(s,(sockaddr*)&temp_addr,(socklen_t*)&length);
    //
    //
    // printf("\nPeer's address is : %s\n",inet_ntoa(temp_addr.sin_addr));
    // printf("\nPeer's port number is : %d\n",(int) ntohs(temp_addr.sin_port));

    puts(buf);

    // while(1)
    // {
    //     printf("Enter message : ");
    //     scanf("%s",&message);
    //
    //     //send the message
    //     if (sendto(s, message, strlen(message) , 0 , (struct sockaddr *) &si_other, slen)==-1)
    //     {
    //         die("sendto()");
    //     }
    //
    //     //receive a reply and print it
    //     //clear the buffer by filling null, it might have previously received data
    //     memset(buf,'\0', BUFLEN);
    //     socklen_t len;
    //     //try to receive some data, this is a blocking call
    //     if (recvfrom(s, buf, BUFLEN, 0, (struct sockaddr *) &si_other, &len) == -1)
    //     {
    //         die("recvfrom()");
    //     }
    //
    //     puts(buf);
    // }

    close(s);
    return 0;
}
