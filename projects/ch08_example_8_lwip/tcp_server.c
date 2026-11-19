#include "lwip_test.h"
#if (TEST_TCP_SERVER)
#include "lwip/sockets.h"
#include "lwip/err.h"
#include "lwip/sys.h"
static char data_buf[TCP_SERVER_BUFSIZE] = "hello, I'm tcp server!\n";
static void tcp_server_thread(void *arg)
{
struct sockaddr_in local_addr;                // Define the remote server address
int sock_fd, err;                            // Define the socket and the error code
sock_fd = socket(AF_INET, SOCK_STREAM, 6);  // Create the socket (level, type, protocol)
if (sock_fd == -1)
{
printk("failed to create sock_fd!\n");
return;
}
else
printk("succeed to create sock_fd!\n");
memset(&local_addr, 0, sizeof(local_addr));    // Allocate space for the local server address
local_addr.sin_family = AF_INET;             // The local server IP consists of two parts
local_addr.sin_addr.s_addr = inet_addr(local_IP); // Local server IP address 192.168.1.123
local_addr.sin_port = htons(TCP_LOCAL_PORT); // Local server port number 9060
local_addr.sin_len = sizeof(local_addr);   // Size of the storage space occupied by the local server address
/***************************** Bind the local server address *******************************/
err = bind(sock_fd, (struct sockaddr *)&local_addr, sizeof(struct sockaddr));
if (err != ERR_OK)
{
closesocket(sock_fd);                   // Binding failed, close the socket
printk("failed to bind ip!\n");
return;
}
else
printk("succeed to bind ip!\n");
err = listen(sock_fd, 3);                    // Start listening
if (err != ERR_OK)
{
closesocket(sock_fd);
printk("failed to listen!\n");
return;
}
else
printk("succeed to listen!\n");
/************************ loop first.*******************************/
while (1)
{
int client_fd;
struct sockaddr_in client_addr;
int addrlen = sizeof(client_addr);
printk("waiting for client access!\n");
client_fd = accept(sock_fd, (struct sockaddr*)&client_addr,
(socklen_t)&addrlen);
if (client_fd > 0)
{
printk("client incoming succeed!\r\n");
/************************loop second***********************/
/*********Loop to receive and automatically forward until the client actively disconnects***********/
for (;;)
{
/******************* Clear the data buffer *******************/
memset(data_buf, 0, TCP_SERVER_BUFSIZE);
/******************* Receive data **************************/
err = recv(client_fd, data_buf, TCP_SERVER_BUFSIZE, 0);
if (err > 0)                        // Receive data normally and forward it
{
printk("RECEIVE: %s \n", data_buf);
send(client_fd, data_buf, err, 0);
printk("    SEND: %s \n", data_buf);
}
else if (err == ERR_CLSD)             // The client connection is interrupted
{
closesocket(client_fd);            // Close the socket
printk("client disconnected.\r\n");
break;
}
else if (err <= 0)                    // The client actively disconnects
{
closesocket(client_fd);
printk("disconnect client succeed!\r\n");
break;
}
}
}
delay_ms(200);
}
/*********************** NEVER GO HERE! *************************************/
closesocket(sock_fd);                          // Close the socket
printk("tcp_server_thread stop!\r\n");            // End
}
void tcp_server_init(void)                        // Initialize the thread
{
sys_thread_new("tcp_server",
tcp_server_thread,
NULL,
DEFAULT_THREAD_STACKSIZE,
DEFAULT_THREAD_PRIO + 1);
}
#endif
