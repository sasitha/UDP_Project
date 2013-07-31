#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <errno.h>
#include <fcntl.h>
#include <arpa/inet.h>


#define MYTCP_PORT 4950
#define MYUDP_PORT 5350
#define MAXSIZE 50
#define DATALEN 100
#define BUFSIZE 60000
#define MAXINT 0x7fffffff

struct ack_so
{	
	int	ack_type ;
	int error_seq_no;
	int error_packet;
	int len;
};

struct packet
{
	char	data[DATALEN];
	int	seq_num;
	int	window_end;
	int	no_of_packets;
	long	packet_number;
        
};	
