#include "lwip_test.h"
#if (TEST_UDP_SERVER)
#include "lwip/sockets.h"
#include "lwip/err.h"
#include "lwip/sys.h"
static char data_buf[UDP_SERVER_BUFSIZE];
static void udp_server_thread(void *arg)
{
struct sockaddr_in local_addr;                // Define the remote server address
int sock_fd,err;                             // Define the socket and the error code
sock_fd = socket(AF_INET, SOCK_DGRAM, 0);   // Create the socket (level, type, protocol)
if (sock_fd == -1)
{
printk("failed to create sock_fd!\n");
return;
}
else
printk("succeed to create sock_fd!\n");
memset(&local_addr, 0, sizeof(local_addr));     // Allocate space for the local server address
local_addr.sin_family = AF_INET;              // The local server IP consists of two parts
/********** Local server IP address 192.168.1.123 ***************/
local_addr.sin_addr.s_addr = inet_addr(local_IP);
local_addr.sin_port = htons(UDP_LOCAL_PORT); // Local server port number 9062
/************************ Bind the local server address ****************************/
err = bind(sock_fd, (struct sockaddr *)&local_addr, sizeof(local_addr));
if (err == -1)
{
closesocket(sock_fd);                // Binding failed, close the socket
printk("failed to bind ip!\n");
return;
}
else
printk("succeed to bind ip!\n");
while (1)
{
struct sockaddr from;
socklen_t fromlen;
memset(data_buf, 0, UDP_SERVER_BUFSIZE);     // Clear the data buffer
/******************** Receive data ********************************/
err = recvfrom(sock_fd, data_buf, UDP_SERVER_BUFSIZE, 0, &from, &fromlen);
if (err > 0)                                  // Receive and forward normally
{
printk("RECEIVE: %s \n", data_buf);
sendto(sock_fd, data_buf, err, 0, &from, fromlen);
printk("    SEND: %s \n", data_buf);
}
else if (err <= 0)                            // Receive error, continue receiving
{
delay_ms(100);
continue;
}
}
/*********************** NEVER GO HERE! ****************************/
closesocket(sock_fd);                             // Close the socket
printk("udp_server_thread stop!\r\n");              // End
}
void udp_server_init(void)                         // Start the thread
{
sys_thread_new("udp_server_thread",
udp_server_thread,
NULL,
DEFAULT_THREAD_STACKSIZE,
DEFAULT_THREAD_PRIO + 1);
}
#endif
