#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <linux/ip.h> /* for ipv4 header */
#include <linux/udp.h> /* for upd header */

#define ADDR_TO_BIND "127.0.0.1"
#define PORT_TO_BIND 8080

#define MSG_SIZE 256
#define HEADER_SIZE (sizeof(struct iphdr) + sizeof(struct udphdr))

struct ip_x {
#if BYTE_ORDER == LITTLE_ENDIAN 
	u_char	ip_hl:4,		/* header length */
		ip_v:4;			/* version */
#endif
#if BYTE_ORDER == BIG_ENDIAN 
	u_char	ip_v:4,			/* version */
		ip_hl:4;		/* header length */
#endif
	u_char	ip_tos;			/* type of service */
	short	ip_len;			/* total length */
	u_short	ip_id;			/* identification */
	short	ip_off;			/* fragment offset field */
#define	IP_DF 0x4000			/* dont fragment flag */
#define	IP_MF 0x2000			/* more fragments flag */
	u_char	ip_ttl;			/* time to live */
	u_char	ip_p;			/* protocol */
	u_short	ip_sum;			/* checksum */
	struct	in_addr ip_src,ip_dst;	/* source and dest address */
};

int main(void) {
    int raw_socket;
    struct sockaddr_in sockstr;
    socklen_t socklen;
    struct ip_x *x;
    char *p;

    int retval = 0; /* the return value (give a look when an error happens)
                     */

    /* no pointer to array!
     * >> It was like "a variable that contains an address -- and in this
     *    address begins an array of chars"! */
    /* now it is simple an array of chars :-)  */
    char msg[MSG_SIZE];
    ssize_t msglen; /* return value from recv() */

    /* do not use IPPROTO_RAW to receive packets */
    if ((raw_socket = socket(AF_INET, SOCK_RAW, IPPROTO_UDP)) == -1) {
        perror("socket");
        return 1; /* here there is no clean up -- retval was not used */
    }

    sockstr.sin_family = AF_INET;
    sockstr.sin_port = htons(PORT_TO_BIND);
    sockstr.sin_addr.s_addr = inet_addr(ADDR_TO_BIND);
    socklen = (socklen_t) sizeof(sockstr);

    /* use socklen instead sizeof()  Why had you defined socklen? :-)  */
    if (bind(raw_socket, (struct sockaddr*) &sockstr, socklen) == -1) {
        perror("bind");
        retval = 1; /* '1' means "Error" */
        goto _go_close_socket;
    }

    memset(msg, 0, MSG_SIZE);

    if ((msglen = recv(raw_socket, msg, MSG_SIZE, 0)) == -1) {
        perror("recv");
        retval = 1;
        goto _go_close_socket;
    }
    
    printf("\nmsg = %s\n",msg);
    
    x = (struct ip_x*)(msg);  
	  printf("\n**************************************************\n");
	  
	  printf("\nVersion : %u",(unsigned int)x->ip_v);
	  printf("\nHeader Length : %u",(unsigned int)x->ip_hl);
	  printf("\nType of service : %u",(unsigned int)x->ip_tos);
	  printf("\nTotal Length : %hu",x->ip_len);
	  printf("\nIdentification : %hu",x->ip_id);
	  printf("\nFragment offset : %hu",x->ip_off);
	  printf("\nTime to live : %u",x->ip_ttl);
	  printf("\nProtocol : %u",x->ip_p	);
	  printf("\nChecksum : %hu",x->ip_sum);
	  //printf("\nSource : %s\nDestination : %s\n",inet_ntoa(x->ip_src),inet_ntoa(x->ip_dst));
	  p = msg + 12;
        
        printf("IP: %d.%d.%d.%d => %d.%d.%d.%d\n",
			p[0]&0XFF, p[1]&0XFF, p[2]&0XFF, p[3]&0XFF,
			p[4]&0XFF, p[5]&0XFF, p[6]&0XFF, p[7]&0XFF);
	  printf("\n**************************************************\n");

    if (msglen <= HEADER_SIZE) /* msg  can't be lesser than header! */
        printf("No msg!\n");
    else {
        msg[msglen - 1] = '\0'; /* we need a null character at the end*/
        printf("Your msg is: %s\n",
               msg + HEADER_SIZE);
    }

_go_close_socket:
    close(raw_socket);

    return retval;
}
