#include <pcap.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
//#include <netinet/in.h>

//#include <netinet/ip.h>
//#include <netinet/tcp.h>
//#include <netinet/if_ether.h>
#include <linux/ip.h>
#include <linux/if_ether.h>
#include <linux/tcp.h>
#include <linux/udp.h>
//#include <netinet/ether.h>


#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>

#define SNAP_LEN 1518

typedef iphdr  ip;
typedef ethhdr ether_header;

void print_eth(ether_header*);
void print_tcp(tcphdr*);
void print_udp(struct updhdr*);
void print_ip(ip*);

void print_eth(ether_header* header)
{
	printf("\n+-Ethernet-----------------------------------------------+\n");
	int n = 0XFF;
	printf("|\t%.2X.%.2X.%.2X.%.2X.%.2X.%.2X   ==>  ",header->h_dest[6]&n,header->h_dest[7]&n,header->h_dest[8]&n,header->h_dest[9]&n,header->h_dest[10]&n,header->h_dest[11]&n);
	printf("%.2X.%.2X.%.2X.%.2X.%.2X.%.2X\n",header->h_dest[0]&n,header->h_dest[1]&n,header->h_dest[2]&n,header->h_dest[3]&n,header->h_dest[4]&n,header->h_dest[5]&n);
	printf("|\t\tProtocol Type = %u",header->h_proto);
	print_ip((ip*)(&(header->h_proto) + 1));
}

void print_tcp(tcphdr *header)
{
	printf("+-TCP----------------------------------------------------+\n");
	printf("|\tSource port : %hu\n",ntohs(header->source));
	printf("|\tDestination port : %hu\n",ntohs(header->dest));
}

void print_udp(struct udphdr * header)
{
	printf("+-UDP----------------------------------------------------+\n");
	printf("|\tSource port : %hu\n",ntohs(header->source));
	printf("|\tDestination port : %hu\n",ntohs(header->dest));
	printf("|\tLength : %hu",ntohs(header->len));
	printf("\t\tChecksum : %hu\n",ntohs(header->check));	
}

void print_ip(ip* header)
{
	printf("\n+-IP-----------------------------------------------------+\n");
	printf("|\tVersion: %u\tHeader Length: %u\n",header->version,header->ihl);
	printf("|\tTotal Length: %hu\n",header->tot_len);
	struct in_addr source,destination;
	char *src = (char*)(&header->saddr);
	printf("|\t%d.%d.%d.%d  ==>  %d.%d.%d.%d\n",src[0]&0XFF,src[1]&0XFF,src[2]&0XFF,src[3]&0XFF,src[4]&0XFF,src[5]&0XFF,src[6]&0XFF,src[7]&0XFF);
	if(header->tot_len != 5)
	{
		printf("|\tIP header contains optional fields.\n");
	}
	printf("|\tProtocol : ");
	switch(header->protocol)
	{
		case IPPROTO_ICMP : printf("ICMP\n"); break;
		case IPPROTO_TCP : printf("TCP\n");print_tcp((tcphdr*)(header + header->ihl));break;
		case IPPROTO_UDP : printf("UDP\n");print_udp((struct udphdr*)(header + header->ihl));break;
		default:
				printf("%hu\n",header->protocol);
	}
	printf("+--------------------------------------------------------+\n");
}

void got_packet(u_char *args,const struct pcap_pkthdr *header,const u_char *packet)
{
	static int packet_number = 1;
	printf("\nPacket Number : %d\n",packet_number);
	tcphdr *tcp_header;
	ether_header *ethernet_header;
	ip *ip_header;
	char *data;

	ethernet_header = (ether_header*)(packet);
	ip_header = (ip*)(packet + sizeof(ether_header));
	tcp_header = (tcphdr*) (packet+sizeof(ether_header) + sizeof(ip));
	data = (char*)(packet+sizeof(ether_header) + sizeof(ip)+sizeof(tcphdr));

	print_eth((ether_header*)packet);
	packet_number++;
}

int main(int argc,char *argv[])
{
	char *dev = NULL;
	char error[PCAP_ERRBUF_SIZE];
	pcap_t *handle;

	char filter_exp[]="ip";
	struct bpf_program fp;
	bpf_u_int32 mask,net;
	int packet_count = 1;
	printf("\nEnter number of packets : ");
	scanf("%d",&packet_count);

	if(argc == 2)
	{
		dev = argv[1];
	}
	else if( argc > 2)
	{
		printf("\n[ERROR] usage %s <interface>\n",argv[0]);
		exit(1);
	}
	else
	{
		dev = pcap_lookupdev(error);
		if(dev == NULL)
		{
			fprintf(stderr,"Can't find any default device [ %s ]\n",error);
			exit(1);
		}
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

	pcap_loop(handle,packet_count,got_packet,NULL);

	pcap_freecode(&fp);
	pcap_close(handle);

	return 0;
}