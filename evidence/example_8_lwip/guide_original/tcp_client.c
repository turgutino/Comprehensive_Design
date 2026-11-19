#include "lwip_test.h"
#if (TEST_TCP_CLIENT)
#include "lwip/sockets.h"
#include "lwip/err.h"
#include "lwip/sys.h"
static char data_buf[TCP_CLIENT_BUFSIZE] = "hello, you are connected!\n";
static void tcp_client_thread(void *arg)
{
struct sockaddr_in remote_addr;             // Define the remote server address
int sock_fd, err, count=0;                   // Define the socket, the error code and the counter
sock_fd = socket(AF_INET, SOCK_STREAM, 0);  // Create the socket (level, type, protocol)
if (sock_fd == -1)
{
printk("failed to create sock_fd!\n");
return;
}
else
printk("succeed to create sock_fd!\n");
memset(&remote_addr, 0, sizeof(remote_addr)); // Allocate space for the remote server address
remote_addr.sin_family = AF_INET;            // The remote server IP consists of two parts
/*************** Remote server IP address 192.168.1.111 ********************/
remote_addr.sin_addr.s_addr = inet_addr(remote_IP);
remote_addr.sin_port = htons(TCP_REMOTE_PORT); // Remote server port number 9061
/*************** Connect to the remote server (socket, address) *******************/
err = connect(sock_fd, (struct sockaddr *)&remote_addr, sizeof(struct sockaddr));
if (err != ERR_OK)
{
closesocket(sock_fd);
printk("failed to connect to server!\n");
return;
}
else
printk("succeed to connect to server!\n");
while (1)
{
int rdbytes = 0;
unsigned int ticks = get_clock_ticks();         // Get the time value
memset(data_buf, 0, TCP_CLIENT_BUFSIZE);   // Clear the data buffer
/****** Add the newline "\n": the receiving end needs to place the string in the data buffer *********/
snprintf(data_buf, 99, "client ticks = %i.\n", ticks);
if (send(sock_fd, data_buf, strlen(data_buf), 0) <= 0)  // Send the data in the data buffer
{
delay_ms(1000);
continue;
}
else
{
printk("SEND: %s", data_buf);
}
memset(data_buf, 0, TCP_CLIENT_BUFSIZE);    // Clear the data buffer
/****************** Receive the data sent by the server ****************************/
rdbytes = recv(sock_fd, data_buf, TCP_CLIENT_BUFSIZE, 0);
if (rdbytes > 0)
{
printk("  RECEIVE: %s \n", data_buf);      // Print the data sent by the server
}
delay_ms(100);
if (++count >= 5)                        // End flag: 5 times of sending and receiving
break;
}
closesocket(sock_fd);                           // Close the socket
printk("tcp_client_thread stop!\r\n");              // End
}
void tcp_client_init(void)                          // Start the thread
{
sys_thread_new("tcp_client",
tcp_client_thread,
NULL,
DEFAULT_THREAD_STACKSIZE,
DEFAULT_THREAD_PRIO + 1);
}
#endif
