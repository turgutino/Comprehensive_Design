#include "lwip_test.h"
#if (TEST_UDP_CLIENT)
#include "lwip/sockets.h"
#include "lwip/err.h"
#include "lwip/sys.h"
static char data_buf[UDP_CLIENT_BUFSIZE] = "this is a UDP test package";
static void udp_client_thread(void *arg)
{
struct sockaddr_in remote_addr;          // Define the remote server address
int sock_fd,err,count = 0;               // Define the socket, the error code and the counter
sock_fd = socket(AF_INET, SOCK_DGRAM, 0);  // Create the socket (level, type, protocol)
if (sock_fd == -1)
{
printk("failed to create sock_fd!\n");
return;
}
else
printk("succeed to create sock_fd!\n");
memset(&remote_addr, 0, sizeof(remote_addr));  // Allocate space for the remote server address
remote_addr.sin_family = AF_INET;            // The remote server IP consists of two parts
/************ Remote server IP address 192.168.1.111 *******************/
remote_addr.sin_addr.s_addr = inet_addr(remote_IP);
remote_addr.sin_port = htons(UDP_REMOTE_PORT);  // Remote server port number 9063
while (1)
{
struct sockaddr from;
socklen_t fromlen;
unsigned int ticks = get_clock_ticks();         // Get the time value
memset(data_buf, 0, UDP_CLIENT_BUFSIZE);   // Clear the data buffer
/************** Put the string in the data buffer *******************/
snprintf(data_buf, UDP_CLIENT_BUFSIZE-1, "client ticks = %i.", ticks);
err = sendto(sock_fd,data_buf,strlen(data_buf), // Send data to the remote server
0,(struct sockaddr *)&remote_addr,
sizeof(remote_addr));
if (err > 0)
printk("SEND: %s\r\n", data_buf);
else if (err <= 0)
{
delay_ms(1000);
continue;
}
delay_ms(100);
memset(data_buf, 0, UDP_CLIENT_BUFSIZE);        // Clear the data buffer
err = recvfrom(sock_fd,data_buf,UDP_CLIENT_BUFSIZE, // Receive data
MSG_WAITALL,&from,&fromlen);
/*************************** MSG_PEEK blocks the first data, MSG_WAITALL blocks every data, MSG_DONTWAIT is the non-blocking mode *****************************************/
if (err > 0)
printk("  RECEIVE: %s\r\n", data_buf);
if (++count >= 5)                              // End flag: 5 times of sending and receiving
break;
}
closesocket(sock_fd);                                 // Close the socket
printk("udp_client_thread stop!\r\n");                   // End
}
void udp_client_init(void)                              // Start the thread
{
sys_thread_new("udp_client_thread",
udp_client_thread,
NULL,
DEFAULT_THREAD_STACKSIZE,
DEFAULT_THREAD_PRIO + 1);
}
#endif
