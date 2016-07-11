#include <sys/socket.h>
#include <linux/netlink.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>

#define NETLINK_USER 30

#define MAX_PAYLOAD 1024
struct sockaddr_nl src_addr;
struct sockaddr_nl dest_addr;
int sock_fd;
struct msghdr msg;
struct nlmsghdr * nlh;
struct iovec iov;

int PrintOut(char * buf, int len);

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
    char buf[8192];
    char *package = NULL;   
    memset(buf,0, sizeof(buf));
    iov.iov_base = buf;
    iov.iov_len = sizeof(buf);
    while(1){
        int status;
        struct nlmsghdr *h ;
        msg = (struct msghdr){
            (void *)&dest_addr, sizeof(dest_addr),
            &iov,1,NULL,0,0
        };
        status = recvmsg(sock_fd, &msg, 0);
        if (status < 0){
            if (errno == EINTR)
            continue;
            continue;
        }
        if (status == 0){
            printf("EOF on netlink\n");
            close(sock_fd);
            return 0;
        }

        h = (struct nlmsghdr *)buf;

        while(NLMSG_OK(h,status)){
            if (h->nlmsg_type == NLMSG_NOOP){
                close(sock_fd);
                printf("NLMSG_DONE\n");
                return 0;
            }
            if (h->nlmsg_type == NLMSG_ERROR){
                struct nlmsgerr *err;
                err = (struct nlmsgerr *)NLMSG_DATA(h);
                fprintf(stderr, "%d Error %d:%s\n", __LINE__, -(err->error), strerror(-(err->error)));
                close(sock_fd);
                printf("NLMSG_ERROR");
                return 0;

            }
            package = (char *)NLMSG_DATA(h);
            PrintOut(package, NLMSG_PAYLOAD(h,0));
            transmit_package(package, NLMSG_PAYLOAD(h,0));
            h = NLMSG_NEXT(h,status);

        }
    }
    close(sock_fd);
}
int transmit_package(unsigned char * buf, int buflen){
   udpsend("127.0.0.1",9009, buf, buflen);
    return 0;
}
int PrintOut(char *buf, int buflen){
        int i;
        printf("Recv: Payload Length=%d,buflen = %d\n", buflen, (int)strlen(buf));

        for (i=0;i<buflen;i++){
            if (i % 8 == 0)
            	printf("\n");
            printf("%02X", buf[i]);
        }
}
