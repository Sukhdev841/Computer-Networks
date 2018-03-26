#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <resolv.h>
#include <string.h>
#include <stdio.h>
#include <sys/un.h>

#define PCKT_LEN 8192

struct ipheader {
	unsigned char      iph_ihl:4;
	unsigned char      iph_ver:4;
	unsigned char      iph_tos;
	unsigned short int iph_len;
	unsigned short int iph_ident;
	unsigned short int iph_offset;
	unsigned char      iph_ttl;
	unsigned char      iph_protocol;
	unsigned short int iph_chksum;
	unsigned int       iph_sourceip;
	unsigned int       iph_destip;
}__attribute__((packed));

struct udpheader {
	unsigned short int udph_srcport;
	unsigned short int udph_destport;
	unsigned short int udph_len;
	unsigned short int udph_chksum;
}__attribute__((packed));

unsigned short csum(unsigned short *buf, int nwords) {
	unsigned long sum;
	for (sum = 0; nwords > 0; nwords--)
		sum += *buf++;
		sum = (sum >> 16) + (sum & 0xffff);
		sum += (sum >> 16);
	return (unsigned short)(~sum);
}

int main() {
	int sd;
	char buffer[65536];
	struct ipheader *ip = (struct ipheader *) buffer;
	struct udpheader *udp = (struct udpheader *) (buffer + sizeof(struct ipheader));
	struct sockaddr_in sin, din;
	char *msg = (char*)(buffer + sizeof(struct ipheader) + sizeof(struct udpheader));
	int one = 1;
	const int *val = &one;
	memset(buffer, 0, PCKT_LEN);
	if ((sd = socket(PF_INET, SOCK_RAW, IPPROTO_UDP)) == -1) {
		perror("socket error");
		exit(1);
	}
	sin.sin_family = AF_INET;
	din.sin_family = AF_INET;
	sin.sin_port = htons(2020);
	din.sin_port = htons(8080);
	sin.sin_addr.s_addr = inet_addr("192.0.0.25");
	din.sin_addr.s_addr = inet_addr("127.0.0.1");

	ip->iph_ihl = 5;
	ip->iph_ver = 4;
	ip->iph_tos = 32;
	ip->iph_len = sizeof(struct ipheader) + sizeof(struct udpheader);
	ip->iph_ident = htons(54321);
	ip->iph_ttl = 64;
	ip->iph_protocol = IPPROTO_UDP;
	ip->iph_sourceip = inet_addr("192.0.0.25");
	ip->iph_destip = inet_addr("127.0.0.1");

	udp->udph_srcport = htons(2020);
	udp->udph_destport = htons(8080);
	udp->udph_len = htons(sizeof(struct udpheader));

	ip->iph_chksum = csum((unsigned short *)buffer, sizeof(struct ipheader) + sizeof(struct udpheader));
	if (setsockopt(sd, IPPROTO_IP, IP_HDRINCL, val, sizeof(one)) < 0) {
		perror("setsockopt() error");
		exit(1);
	}
	strcat(msg,"sukhdev");
	for (int count = 1; count <= 20; count++) {		
		printf("\nSending : %s\n",buffer);
		if (sendto(sd, buffer, sizeof(struct ipheader) + sizeof(struct udpheader) + sizeof("sukhdev"), 0, (struct sockaddr *)&din, sizeof(din)) < 0) {
			perror("sendto() error");
			exit(-1);
		} else {
			printf("Count #%u - sendto() is OK.\n", count);
			sleep(2);
		}
	}
	close(sd);
	return 0;
}
