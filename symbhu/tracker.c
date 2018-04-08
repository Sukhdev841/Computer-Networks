#include <pcap.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include <linux/ip.h>
#include <linux/if_ether.h>
#include <linux/tcp.h>
#include <linux/udp.h>

#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <errno.h>
#include <bits/stdc++.h>

using namespace std;

vector<int> sent_ports;

#define SNAP_LEN 1518

typedef iphdr  ip;
typedef ethhdr ether_header;

char *peer_ip;

#define global_fifo "my_fifo"
int fd;

void print_eth(ether_header*);
void print_tcp(tcphdr*);
void print_ip(ip*);
int is_my_packet(ip*,char*);

void print_eth(ether_header* header)
{
//	printf("\n+-Ethernet-----------------------------------------------+\n");
	int n = 0XFF;
//	printf("|\t%.2X.%.2X.%.2X.%.2X.%.2X.%.2X   ==>  ",header->h_dest[6]&n,header->h_dest[7]&n,header->h_dest[8]&n,header->h_dest[9]&n,header->h_dest[10]&n,header->h_dest[11]&n);
//	printf("%.2X.%.2X.%.2X.%.2X.%.2X.%.2X\n",header->h_dest[0]&n,header->h_dest[1]&n,header->h_dest[2]&n,header->h_dest[3]&n,header->h_dest[4]&n,header->h_dest[5]&n);
//	printf("|\t\tProtocol Type = %u",header->h_proto);
}

void print_tcp(tcphdr *header)
{
//	printf("+-TCP----------------------------------------------------+\n");
//	printf("|\t\t\t%u ==> ",ntohs(header->source));
//	printf("%u\n",ntohs(header->dest));
//	printf("|\t\t\tSYN = %d\t ACK = %d\n",header->syn,header->ack);
	if(header->syn && header->ack)
	{
	//	printf("|\t\tServer Responding to Connection.\n");
		char msg[100];
		for(int i=0;i<sent_ports.size();i++)
		{
			if(sent_ports[i] == ntohs(header->source) )
				return;
		}
		sprintf(msg,"%d\n",ntohs(header->source));
		write(fd,msg,strlen(msg)+1);
		printf("\nport sent to the symbhu %s\n",msg);
		sent_ports.push_back(ntohs(header->source));
		//printf("%d\n",ntohs(header->source));
		//printf("\nSatisfied\n");
	}
	else if(header->syn && !header->ack)
	{
	//	printf("|\t\tClient Wants to connect.\n");
	//	printf("%d",ntohs(header->source));
		//fprintf(stderr,"Sending port : %d",ntohs(header->source));
		//printf("%d",ntohs(header->source));
	}
	else if(!header->syn && header->ack)
	{
		//printf("|\t\tClient sending final acknowledgement.\n");
	}
}

void print_ip(ip* header)
{
	int flag = 0;
//	printf("\n+-IP-----------------------------------------------------+\n");
//	printf("|\tVersion: %u\tHeader Length: %u\n",header->version,header->ihl);
//	printf("|\tTotal Length: %u\n",ntohs(header->tot_len));
	struct in_addr source,destination;
	char *src = (char*)(&header->saddr);
	//printf("|\t%d.%d.%d.%d  ==>  %d.%d.%d.%d\n",src[0]&0XFF,src[1]&0XFF,src[2]&0XFF,src[3]&0XFF,src[4]&0XFF,src[5]&0XFF,src[6]&0XFF,src[7]&0XFF);
//	printf("|\tProtocol : ");
	// switch(header->protocol)
	// {
	// 	case IPPROTO_ICMP : //printf("ICMP\n"); break;
	// 	case IPPROTO_TCP : //printf("TCP\n");break;
	// 	default:
	// 		//	printf("%hu\n",header->protocol);
	// }
}

void got_packet(u_char *args,const struct pcap_pkthdr *header,const u_char *packet)
{
	static int packet_number = 1;
	ip *ip_header;
	tcphdr *tcp_header = (tcphdr*)(packet + sizeof(ether_header) + sizeof(ip));
	ip_header = (ip*)(packet + sizeof(ether_header));
	char * data = (char*)(packet + sizeof(ether_header) + sizeof(ip) + sizeof(tcphdr));
	if(ip_header->protocol == IPPROTO_TCP && is_my_packet(ip_header,"172.30.100.178"))
	{
		// process only TCP packets
		//printf("\nPacket Number : %d\n",packet_number);
		print_eth((ether_header*)packet);
		print_ip(ip_header);
		print_tcp((tcphdr*)(packet + sizeof(ether_header) + sizeof(ip)));
	//	printf("+--------------------------------------------------------+\n");
	//	printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
		int count = (ntohs(ip_header->tot_len)) - sizeof(tcphdr) - sizeof(ip);
	//	printf("\nTCP size = %d, IP size = %d\n",sizeof(tcphdr),sizeof(ip));
		while(count > 0)
		{
		//	printf("%c%c",data[0],data[1]);
			data += 2;
			count -=2;
		}
		//printf("\nMSG length %d\n",strlen(data));
	//	printf("$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n");
		packet_number++;
	}
}

int is_my_packet(ip* header,char *my_ip)
{
	char *src = (char*)(&header->saddr);
	char other_IP[20];
	sprintf(other_IP,"%d.%d.%d.%d",src[0]&0XFF,src[1]&0XFF,src[2]&0XFF,src[3]&0XFF);
	if(!strcmp(other_IP,my_ip))
		return 1;
	sprintf(other_IP,"%d.%d.%d.%d",src[4]&0XFF,src[5]&0XFF,src[6]&0XFF,src[7]&0XFF);
	if(!strcmp(other_IP,my_ip))
		return 1;
	return 0;

}

int main(int argc,char *argv[])
{
	fd = open(global_fifo,O_WRONLY);
	printf("\nFifo opened with %d\n",fd);
	char *dev = NULL;
	char error[PCAP_ERRBUF_SIZE];
	pcap_t *handle;

	char filter_exp[]="ip";
	struct bpf_program fp;
	bpf_u_int32 mask,net;
	int packet_count = 1;

	// if(argc < 2)
	// {
	// //	printf("\n[ERROR] usage %s <peer IP>\n",argv[0]);
	// 	exit(1);
	// }
	if(argc == 2)
	{
		peer_ip = argv[1];
	}
	else if( argc > 2)
	{
		printf("\n[ERROR] usage %s <peer IP>\n",argv[0]);
		exit(1);
	}
	dev = pcap_lookupdev(error);
	if(dev == NULL)
	{
		fprintf(stderr,"Can't find any default device [ %s ]\n",error);
		exit(1);
	}

	if(pcap_lookupnet(dev,&net,&mask,error) == -1)
	{
		fprintf(stderr,"Couldn't get IP and mast for device[ %s ].\nSetting to default.\n",error);
		net = mask = 0;
	}

	printf("Device : %s\n",dev);
	printf("Number of packets : %d\n",packet_count);
	printf("Filter expression : %s\n",filter_exp);

	handle = pcap_open_live(dev,SNAP_LEN,1,1000,error);
	if(handle == NULL)
	{
		fprintf(stderr,"Couldn't get handle %s.\n",error);
		exit(0);
	}

	if(pcap_datalink(handle) != DLT_EN10MB)
	{
		fprintf(stderr,"%s is not an Ethernet.\n",dev);
		exit(1);
	}

	if(pcap_compile(handle,&fp,filter_exp,0,net) == -1)
	{
		printf("\nFilter error\n");
		exit(1);
	}

	if(pcap_setfilter(handle,&fp) == -1)
	{
		printf("\nInstalling filter error.\n");
		exit(1);
	}

	while(true)
		pcap_loop(handle,packet_count,got_packet,NULL);

	pcap_freecode(&fp);
	pcap_close(handle);

	return 0;
}