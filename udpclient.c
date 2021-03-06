/*************************************************************************
	> File Name: udpclient.c
	> Author: 
	> Mail: 
	> Created Time: 2016年07月11日 星期一 11时49分29秒
 ************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netdb.h>
#include <sys/socket.h>

#define PORT 9009
int packet_print(const char * buf, int len){

	int i;
	for (i=0;i<len;i++){
		printf("%02X-", buf[i]);
	}
	printf("\n");
	return 0;
}

int udpsend(const char *host, int port,const char * buf, int len)
{
        struct sockaddr_in myaddr, remaddr;
        int fd, i, slen=sizeof(remaddr);
        char *server = (char *)host;/* change this to use a different server */

        /* create a socket */

        if ((fd=socket(AF_INET, SOCK_DGRAM, 0))==-1)
            printf("socket created\n");

        /* bind it to all local addresses and pick any port number */

        memset((char *)&myaddr, 0, sizeof(myaddr));
        myaddr.sin_family = AF_INET;
        myaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        myaddr.sin_port = htons(0);

        if (bind(fd, (struct sockaddr *)&myaddr, sizeof(myaddr)) < 0) {
                perror("bind failed");
                return 0;
            
        }       

        /* now define remaddr, the address to whom we want to send messages */
        /* For convenience, the host address is expressed as a numeric IP address */
        /* that we will convert to a binary format via inet_aton */

        memset((char *) &remaddr, 0, sizeof(remaddr));
        remaddr.sin_family = AF_INET;
        remaddr.sin_port = htons(port);
        if (inet_aton(server, &remaddr.sin_addr)==0) {
                fprintf(stderr, "inet_aton() failed\n");
                exit(1);
            
        }

        /* now let's send the messages */

        printf("Sending packet to %s port %d ----->\n", server, port);
	packet_print(buf,len);
	printf("<---------\n");
        if (sendto(fd, buf, len, 0, (struct sockaddr *)&remaddr, slen)==-1) {
            perror("sendto");
            exit(1);
        }
#if 1
	char recebuf[1024];
	int BUFLEN=1024;
	int recvlen ;
                /* now receive an acknowledgement from the server */
        recvlen = recvfrom(fd, recebuf, BUFLEN, 0, (struct sockaddr *)&remaddr, &slen);
        if (recvlen >= 0) {
            recebuf[recvlen] = 0;/* expect a printable string - terminate it */
            printf("received message: \"%s\"\n", recebuf);
                            
        }
#endif            
    close(fd);
        return 0;

}
