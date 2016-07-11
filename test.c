#include <sys/socket.h>
#include <linux/netlink.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define NETLINK_USER 30

#define MAX_PAYLOAD 1024
struct sockaddr_nl src_addr, dest_addr;
struct nlmsghdr *nlh = NULL;
struct iovec iov;
int sock_fd;
struct msghdr msg;

int main()
{
        sock_fd=socket(PF_NETLINK, SOCK_RAW, NETLINK_USER);
        if(sock_fd<0)
                return -1;

        memset(&src_addr, 0, sizeof(src_addr));
        src_addr.nl_family = AF_NETLINK;
        src_addr.nl_pid = getpid();

        bind(sock_fd, (struct sockaddr*)&src_addr, sizeof(src_addr));

        memset(&dest_addr, 0, sizeof(dest_addr));
        memset(&dest_addr, 0, sizeof(dest_addr));
        dest_addr.nl_family = AF_NETLINK;
        dest_addr.nl_pid = 0;
        dest_addr.nl_groups = 0;

        nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
        memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));
    nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
        nlh->nlmsg_pid = getpid();
        nlh->nlmsg_flags = 0;

        strcpy(NLMSG_DATA(nlh), "Hello");

        iov.iov_base = (void *)nlh;
        iov.iov_len = nlh->nlmsg_len;
        msg.msg_name = (void *)&dest_addr;
        msg.msg_namelen = sizeof(dest_addr);
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;

        printf("Sending message to kernel\n");
        sendmsg(sock_fd,&msg,0);
        printf("Waiting for message from kernel\n");

    int ret = 0;
    while(1){
        memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));

        iov.iov_base = (void *)nlh;
        iov.iov_len = NLMSG_SPACE(MAX_PAYLOAD);

        msg.msg_name = (void *)&dest_addr;
        msg.msg_namelen = sizeof(dest_addr);
        msg.msg_iov = &iov;
        msg.msg_iovlen = 1;
        
        
        ret = recvmsg(sock_fd, &msg, 0);
        if (ret == 0){
            printf("Not get data\n");
            close(sock_fd);
        }else if (ret == -1){
            perror("recvmsg:");
            close(sock_fd);
        }
        char *buf = (char *)NLMSG_DATA(nlh);
        int buflen = ret - NLMSG_LENGTH(0);
        int i ;
        printf("Recv: Payload Length=%d,buflen = %d\n", buflen, strlen(buf));

        //printf("Received message payload: %s\n\n", (char *)NLMSG_DATA(nlh));
        for (i=0;i<buflen;i++){
            printf("%02X", buf[i]);
            if (i % 8 == 0)
            printf("\n");
        }
    }
        close(sock_fd);
}
