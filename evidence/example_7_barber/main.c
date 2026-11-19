/*
 * RThread6 - Sleeping-Barber problem (guide section 7, Listing 11-59).
 * Guide code is unchanged; only these #include lines come from the
 * LoongIDE RT-Thread template main.c (the listing has none).
 */

#include <time.h>

#include "rtthread.h"
#include "bsp.h"

#define THREAD_PRIORITY     10               // Define the thread priority
#define THREAD_STACK_SIZE   1024*20          // Define the thread stack size
#define THREAD_TIMESLICE    5                // Define the thread time-slice length
#define customer_num        5                // Define the total number of customers
static rt_sem_t customers=NULL;                 // Define the semaphore customers
static rt_sem_t barber=NULL;                    // Define the semaphore barber
static rt_mutex_t mutex=NULL;                   // Define the mutex mutex
static rt_thread_t barber_tid = NULL;               // Define 1 thread handle barber_tid
static rt_thread_t customer_tid[customer_num];     // Define 5 thread handles
int num[customer_num]={0,1,2,3,4};               // Define the customer-number recording array num
static unsigned char freeseats=3;                 // Define the number of available chairs, with an initial value of 3
static void barber_thread(void *arg)               // Thread function barber_thread
{
static rt_err_t res;                           // Define the variable res
static rt_uint8_t number=0;                  // Define the number of customers served by the barber
int i;
while(1)                                    // Infinite loop
{
/* Acquire the semaphore customers, wait for a customer to appear */
res=rt_sem_take(customers,RT_WAITING_FOREVER);
/* If acquiring the semaphore fails, print the failure information and delete the semaphore customers */
if(res!=RT_EOK){
rt_kprintf("barber thread take a dynamic semaphore failed.\n");
rt_sem_delete(customers);
return;
}
/* If acquiring the semaphore succeeds, print the information that the barber finds a customer */
else{
rt_kprintf("barber find customer %d .\n", number);
}
number++;                                // The number of customers served by the barber increases by 1
rt_mutex_take(mutex,RT_WAITING_FOREVER);   // Acquire the mutex and enter the critical section
freeseats++;                               // The number of empty chairs increases by 1
rt_sem_release(barber);                      // Release the semaphore barber
rt_mutex_release(mutex);                     // Release the mutex mutex
rt_kprintf("barber begin to cut hair.\n");         // Print the information that the barber begins to cut hair
rt_thread_mdelay(40);                        // Simulate the time needed for the barber to cut hair
rt_kprintf("barber finish cut hair.\n");            // Print that the barber has finished cutting the customer's hair
}
}
/* The following consumer thread customer_thread distinguishes the thread corresponding to each customer by the entry parameter num */
static void customer_thread(int num)
{
static rt_err_t res;                             // Define the result variable
rt_thread_mdelay(50);                         // Simulate the arrival time of the customer
rt_mutex_take(mutex,RT_WAITING_FOREVER);    // Acquire the mutex and enter the critical section
if(freeseats>0){                              // If there is an empty seat
freeseats--;                             // The number of empty seats decreases by 1
rt_kprintf("freeseats=%d\n", freeseats);       // Print the information of the number of empty chairs
rt_sem_release(customers);                // Release the semaphore customers
rt_mutex_release(mutex);                  // Release the mutex mutex
/* The following line of code prints the information about which customer is waiting for a haircut */
rt_kprintf("customers %d is waitting to cut hair.\n",num+1);
res=rt_sem_take(barber,RT_WAITING_FOREVER); // Acquire the semaphore barber
}
else{                                        // If there is no empty chair
rt_kprintf("customers %d leave.\n",num+1); // Print the information that the customer leaves the barbershop
rt_mutex_release(mutex);               // Release the mutex
}
}
/*---------------------------------------------------------------------*/
int main(int argc, char** argv)
{
int i=0;                                   // Initialize the variable i
int j=0;                                   // Initialize the variable j
rt_kprintf("\r\nWelcome to RT-Thread.\r\n\r\n");
ls1x_drv_init();                             // Initialize the internal and external peripheral controllers of the processor
rt_ls1x_drv_init();                           // Initialize the peripheral controllers for RTT
install_3th_libraries();                       // Install the third-party libraries
for (i=0;i<customer_num;i++)               // Initialize the 5 customer thread handles to NULL
customer_tid[i]=NULL;
/* The following code creates the semaphore barber */
barber=rt_sem_create("barber_sem",0,RT_IPC_FLAG_PRIO);
if(barber!=NULL)
rt_kprintf("create semaphore barber done.  The value of barber = 0\n");
/* The following code creates the semaphore customers */
customers=rt_sem_create("cust_sem",0,RT_IPC_FLAG_PRIO);
if(customers!=NULL)
rt_kprintf("create semaphore customers done. The value of customers = 0\n");
/* The following code creates the mutex mutex */
mutex=rt_mutex_create("seatmutex",RT_IPC_FLAG_PRIO);
if(mutex !=NULL)
rt_kprintf("create mutex is sucessful. \n");
/* The following code creates the barber thread */
barber_tid = rt_thread_create("barber",             // Name
barber_thread,       // Thread function
NULL,               // Parameter (empty)
THREAD_STACK_SIZE,  // Stack size
THREAD_PRIORITY,    // Priority
THREAD_TIMESLICE);   // Time slice
/* The following code starts the barber thread */
if (barber_tid != NULL){
rt_thread_startup(barber_tid);
rt_kprintf("barber thread  startup. \n");
}
/* The following code creates 5 customer threads */
for(i=0;i<customer_num;i++)
{
/* The following line of code calculates the priority of different customer threads according to the customer number */
j=THREAD_PRIORITY-5+i;
customer_tid[i]= rt_thread_create("customer",             // Name
customer_thread,       // Thread function
num[i],                // Entry parameter
THREAD_STACK_SIZE,   // Stack size
j,                     // Priority
THREAD_TIMESLICE);    // Time slice
/* The following code starts the different customer threads and prints the corresponding information */
if (customer_tid[i] != NULL){
rt_thread_startup(customer_tid[i]);
rt_kprintf("customers %d thread  startup. \n", i+1);
}
}
return 0;
}
