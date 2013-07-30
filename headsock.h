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

#define MYTCP_PORT 4950
#define MYUDP_PORT 5350
#define MAXSIZE 50
#define DATALEN 100
#define BUFSIZE 60000
#define MAXINT 0x7fffffff

struct ack_so
{	
	uint8_t	ack_type ;
	uint8_t error_seq_no;
	uint8_t error_packet;
	uint8_t len;
};

struct packet
{
	char	data[DATALEN];
	int	seq_num;
	int	window_end;
	int	no_of_packets;
	int	packet_number;
};	
