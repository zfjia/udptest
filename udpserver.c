/*************************************************************************
	> File Name: udpserver.c
	> Author: 
	> Mail: 
	> Created Time: 2016年07月11日 星期一 11时42分10秒
 ************************************************************************/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include "udpclient.h"
#include "netlinkclient.h"

#define PORT 9009
#define BUFSIZE 8192

int
main(int argc, char **argv)
{
        struct sockaddr_in myaddr;      /* our address */
        struct sockaddr_in remaddr;     /* remote address */
        socklen_t addrlen = sizeof(remaddr);            /* length of addresses */
        int recvlen;                    /* # bytes received */
        int fd;                         /* our socket */
        unsigned char buf[BUFSIZE];     /* receive buffer */

            /* create a UDP socket */

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("cannot create socket\n");
        return 0;
                
    }

            /* bind the socket to any valid IP address and a specific port */

    memset((char *)&myaddr, 0, sizeof(myaddr));
    myaddr.sin_family = AF_INET;
    myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    myaddr.sin_port = htons(PORT);

    if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
        perror("bind failed");
        return 0;
    }

    /* now loop, receiving data and printing what we received */
    for (;;) {
	memset(buf, 0, sizeof(buf));
        printf("waiting on port %d\n", PORT);
        recvlen = recvfrom(fd, buf, BUFSIZE, 0, (struct sockaddr *)&remaddr, &addrlen);
        printf("received %d bytes\n", recvlen);
        if (recvlen > 0) {
            buf[recvlen] = 0;
            printf("received message: \"%s\"\n", buf);
		packet_print(buf,recvlen);
		send_kernel(buf,recvlen);
        }
	memset(buf, 0, sizeof(buf));
	sprintf(buf,"I got it! This is ACK!");
	sendto(fd, buf, BUFSIZE, 0,(struct sockaddr *)&remaddr, addrlen);
                
    }
    return 0;
}
