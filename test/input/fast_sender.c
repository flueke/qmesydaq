/*
 Simple udp server
 */
#include<stdio.h> //printf
#include<string.h> //memset
#include<stdlib.h> //exit(0);
#include<arpa/inet.h>
#include<sys/socket.h>
#include <errno.h>
#include <unistd.h>
#include <netdb.h>
#include <netinet/in.h>
#include <time.h>
#include <inttypes.h>
#define BUFLEN 1434  //Max length of buffer to avoid udp fragmentation
#define PORT 8888   //The port on which to listen for incoming data
#define EVENTS 232
#define DLEN 1500L


struct sock
{
    int s;
    struct addrinfo* res;
    struct sockaddr_storage src;
};

unsigned long bcount;

struct MCPDHeader
{
    uint16_t btype;
    uint16_t hlength;
    uint16_t BufferNumber;
    uint16_t RunID;
    uint8_t MCPDID;
    uint8_t Status;
    uint16_t headts[3]; // :48;
    uint16_t param0[3]; // :48;
    uint16_t param1[3]; // :48;
    uint16_t param2[3]; // :48;
    uint16_t param3[3]; // :48;
};

struct neutronevent
{
#if 0
    uint8_t id :1;
    uint8_t modid :3;
    uint8_t slotid :5;
    uint16_t amplitude :10;
    uint16_t position :10;
    uint32_t ts :19;
#endif
    uint16_t data[3];
};

struct triggerevent
{
#if 0
    uint8_t id :1;
    uint8_t trigid :3;
    uint8_t dataid :4;
    uint32_t data :21;
    uint32_t ts :19;
#endif
    uint16_t data[3];
};

union event
{
    struct neutronevent neutronevent;
    struct triggerevent triggerevent;
};

struct packet
{
    uint16_t buflen;
    struct MCPDHeader header;
    union event events[EVENTS];
};

void setup_socket(struct sock* sock);
int  set_buffer_size(int s);
void setup_cmd_socket(struct sock* sock);
void wait_for_command(struct sock * s);
void senddata(struct sock* sock);
void send_finished(struct sock * s);
void finish(struct sock* sock);
void* gendata(void);

void die(char *s)
{
    perror(s);
    exit(1);
}
void warn(char* msg) {
    printf("%s\n", msg);
}

unsigned char *data ;
unsigned int quit = 0;

int main(int argc, char** argv)
{
    struct sock sock, cmd_sock;
    printf("%ld %ld\n", sizeof(struct packet), sizeof(union event));
    printf("%ld %ld\n", sizeof(struct neutronevent), sizeof(struct neutronevent));
    printf("%ld %ld %ld\n", sizeof(struct MCPDHeader), sizeof(union event) * EVENTS, sizeof(uint16_t));
    data = (unsigned char *)gendata();
    printf("data ready\n");
    // setup_cmd_socket(&cmd_sock);
    printf("cmd socket ready\n");
    setup_socket(&sock);
    printf("data socket ready");
    getchar();
    for (quit=0; quit !=1;) {
        wait_for_command(&cmd_sock);
        senddata(&sock);
        // send_finished(&cmd_sock);
    }
    finish(&sock);
    finish(&cmd_sock);
}


void setup_socket(struct sock *sock)
{

    int s, i;

    const char* hostname = "taco6.taco.frm2"; // "opc-158.office.frm2"; /* localhost */
    const char* portname = "54321"; // "15555";
    struct addrinfo hints;
    struct addrinfo* res = 0;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = 0;
    hints.ai_flags = AI_ADDRCONFIG;
    int err = getaddrinfo(hostname, portname, &hints, &res);
    if (err != 0)
    {
        die("failed to resolve remote socket address ");
    }

    s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (s == -1)
    {
        die("Socket creation failed");
    }
    if (set_buffer_size(s) == -1 ) {
        die("Setting buffer size failed");
    }      
    if (connect(s, res->ai_addr, res->ai_addrlen) == -1)
    {
        die("connect()");
    }
    sock->s = s;
    sock->res = res;
}

int set_buffer_size(int s)
{
    int rc = 0;
    int sndbuf_actual, rcvbuf_actual;

    /*
     * Set socket buffer size if requested.  Do this for both sending and
     * receiving so that we can cover both normal and --reverse operation.
     */
    int opt = 16 * 1024*1024; /* 16 MB */
    socklen_t optlen;
    
    if (setsockopt(s, SOL_SOCKET, SO_RCVBUF, &opt, sizeof(opt)) < 0) {
        return -1;
    }
    if (setsockopt(s, SOL_SOCKET, SO_SNDBUF, &opt, sizeof(opt)) < 0) {
        return -1;
    }

    /* Read back and verify the sender socket buffer size */
    optlen = sizeof(sndbuf_actual);
    if (getsockopt(s, SOL_SOCKET, SO_SNDBUF, &sndbuf_actual, &optlen) < 0) {
	return -1;
    }
    printf("SNDBUF is %u, expecting %u\n", sndbuf_actual, opt);

    /* Read back and verify the receiver socket buffer size */
    optlen = sizeof(rcvbuf_actual);
    if (getsockopt(s, SOL_SOCKET, SO_RCVBUF, &rcvbuf_actual, &optlen) < 0) {
	return -1;
    }
    printf("RCVBUF is %u, expecting %u\n", rcvbuf_actual, opt);
}


void setup_cmd_socket(struct sock *sock)
{

    int s, i;

    const char* hostname = "0.0.0.0"; /* wildcard */
    const char* portname = "54321"; // "15556";
    struct addrinfo hints;
    struct addrinfo* res = 0;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_protocol = 0;
    hints.ai_flags = AI_ADDRCONFIG;
    int err = getaddrinfo(hostname, portname, &hints, &res);
    if (err != 0)
    {
        die("failed to resolve remote socket address ");
    }

    s = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (s == -1)
    {
        die("Socket creation failed");
    }
    if (bind(s,res->ai_addr,res->ai_addrlen)==-1) {
        die("cmd sockect bind failed");
    }
    sock->s = s;
    sock->res = res;
}


void handle_command(char* msg, ssize_t count)
{

    printf("%s\n", msg);
    if (msg == "EXIT") {
        quit=1;
    }
    return;


}

void wait_for_command(struct sock * s){
    char buffer[548];

    struct iovec iov[1];
    iov[0].iov_base=buffer;
    iov[0].iov_len=sizeof(buffer);

    struct msghdr message;
    message.msg_name=&(s->src);
    message.msg_namelen=sizeof(s->src);
    message.msg_iov=iov;
    message.msg_iovlen=1;
    message.msg_control=0;
    message.msg_controllen=0;

    printf("waiting for start\n");
    return;
    ssize_t count=recvmsg(s->s,&message,0);
    if (count==-1) {
        die("receive on cmd socket failed");
    } else if (message.msg_flags&MSG_TRUNC) {
        warn("datagram too large for buffer: truncated");
    } else {
        handle_command(buffer,count);
    }
}

void send_finished(struct sock * s)
{
    char buf[256] ="STOP";
    printf("Send finish");
    sendto(s->s, buf, strlen(buf), 0,
            (struct sockaddr *)&s->src, sizeof(s->src));
}

void genevent(union event *evin, uint64_t headts)
{
    struct timespec ts2;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts2);

    struct neutronevent *ev = &(evin->neutronevent);
    uint64_t tim = (uint64_t)((ts2.tv_sec * 1e9 + ts2.tv_nsec) - headts*100);
#if 0
    ev->id = 0;
    ev->modid = 1;
    ev->slotid = 2;
    ev->amplitude = 100;
    ev->position = 200;
    ev->ts = 
#endif
    uint8_t id = 0;
    uint8_t modid = 1;
    uint8_t slotid = 2;
    uint16_t position = 200;
    uint16_t amplitude = 100;
    ev->data[0] = tim & 0xffff;
    ev->data[1] = ((tim >> 16) & 0x7) + ((position & 0x3ff) << 3) + (amplitude & 0x7);
    ev->data[2] = ((amplitude >> 3) & 0x7f) + ((slotid & 0x1f) << 7) + ((modid & 0x7) << 12) + ((id & 1) << 15);
}

struct MCPDHeader genheader(void)
{
    struct timespec ts2;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts2);
    struct MCPDHeader header;
    header.btype = 0;
    header.hlength = 21;
    header.BufferNumber = bcount++;
    header.RunID = 10;
    header.MCPDID = 1;
    header.Status = 1;
    uint64_t t = (uint64_t)(ts2.tv_sec * 1e7 + ts2.tv_nsec * 100);
    for (int i = 0; i < 3; ++i)
	header.headts[0] = 0xFF & (t >> (1 << i));
    header.param0[0] = 0;
    header.param1[0] = 1;
    header.param2[0] = 2;
    header.param3[0] = 3;
    return header;
}

struct packet genpacket(void) {
    struct packet packet;
    packet.header = genheader();
    for (int i=0; i<EVENTS; i++) {
        genevent( &packet.events[i], packet.header.headts );
    }
    packet.buflen = packet.header.hlength + EVENTS * 6 + 1;
    return packet;
}

void* gendata(void)
{
    size_t psize = 42 + (EVENTS * 12);
    struct packet *pdata = (struct packet*) malloc(DLEN*(sizeof( struct packet)));
    if (pdata == NULL)
    {
        die("memory");
    }
    printf ("Generate %ld packets with %ld events\n", DLEN, (DLEN * EVENTS));
    for (int i = 0; i < DLEN; i++)
    {
        pdata[i] = genpacket();
    }
    printf ("Data generation done");
    return (void *)pdata;
}

void senddata(struct sock *sock)
{
    int count = 10;
    struct addrinfo* res = sock->res;
    struct timespec ts1, tw1; // both C11 and POSIX
    struct timespec sl = { 0, 200L };
    unsigned long sent = 0;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts1); // POSIX
    clock_gettime(CLOCK_MONOTONIC, &tw1); // POSIX; use timespec_get in C11
    while (count--)
    {
        // now reply the client with the same data
	struct packet *p = data;
        for (size_t start = 0; start < DLEN; ++p, ++start)
        {
	    ssize_t sb = send(sock->s, p, BUFLEN, 0);
            if (sb != BUFLEN)
            {
                // die("send");
		count = 0;
		break;
            }
            sent += BUFLEN;
            nanosleep(&sl, NULL);
        }
    }
    struct timespec ts2, tw2;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts2);
    clock_gettime(CLOCK_MONOTONIC, &tw2);

    double posix_dur = 1000.0 * ts2.tv_sec + 1e-6 * ts2.tv_nsec
            - (1000.0 * ts1.tv_sec + 1e-6 * ts1.tv_nsec);
    double posix_wall = 1000.0 * tw2.tv_sec + 1e-6 * tw2.tv_nsec
            - (1000.0 * tw1.tv_sec + 1e-6 * tw1.tv_nsec);

    printf("CPU time used (per clock_gettime()): %.2f ms\n", posix_dur);
    printf("Wall time passed: %.2f ms\n", posix_wall);
    printf("Bytes sent: %ld, rate: %.2f kB/sec\n", sent, sent / posix_wall);
    uint32_t packets = sent / BUFLEN;
    printf("Packets sent: %ld, rate: %.2f pkg/sec\n", packets, packets / (posix_wall / 1000.));
    printf("Events sent %ld: rate: %.2f Ev/sec\n", packets * EVENTS, packets*EVENTS / (posix_wall / 1000.));
    // free(data);
}

void finish(struct sock *sock)
{
    close(sock->s);
}
