CC=gcc
CFLAGS = -g 
#
all: udpserver recv
#
udpserver:
	$(CC) -o udpserver udpserver.c udpclient.c netlinkclient.c $(LIBS)
recv:  
	$(CC) -o recv recv.c udpclient.c  $(LIBS)
clean:
	rm -f recv udpserver
