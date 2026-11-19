/*
 * example_8_lwip - guide section 8 (LwIP network transmission), Listing 10-1.
 * Guide code unchanged; these #include lines come from the LoongIDE
 * RT-Thread template main.c (the listing has none).
 */

#include <time.h>

#include "rtthread.h"
#include "bsp.h"

#ifdef BSP_USE_GMAC0
static unsigned char gmac0_ip[4] = {192, 168, 1, 123};
extern void ls1x_initialize_lwip(unsigned char *ip0, unsigned char *ip1);
#endif
//-----------------------------------------------------------------------
// Simple demo of task
//-----------------------------------------------------------------------
#include "lwip_test.h"
int main(int argc, char** argv)
{
rt_kprintf("\r\nWelcome to RT-Thread.\r\n\r\n");
#ifdef BSP_USE_GMAC0
{
/* Initialize the local IP address. Parameters (GMAC0, GAC1). When the parameters are empty, the default address is used */
ls1x_initialize_lwip(gmac0_ip, NULL);
#if (TEST_TCP_SERVER)
{
extern void tcp_server_init(void);
tcp_server_init();                     // Initialize the tcp server
}
#endif
#if (TEST_TCP_CLIENT)
{
extern void tcp_client_init(void);
tcp_client_init();                     // Initialize the tcp client
}
#endif
#if (TEST_UDP_SERVER)
{
extern void udp_server_init(void);
udp_server_init();                  // Initialize the udp server
}
#endif
#if (TEST_UDP_CLIENT)
{
extern void udp_client_init(void);
udp_client_init();                  // Initialize the udp client
}
#endif
}
#endif
return 0;
}
