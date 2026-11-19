/*
 * reflex_test - own design ("own design" task of the course)
 *
 * Reaction-time test over the serial console, Loongson 1B (LS1B200),
 * bare-metal project built with LoongIDE 1.1 / SDE Lite 4.9.2.
 *
 * No LEDs, no RTOS, no dynamic memory and no printf.
 *
 * How it works:
 *   R  - arm one measurement: the board waits a random 1..3 s;
 *   GO - the cue; the milliseconds between the cue and the first key pressed
 *        are the reaction time;
 *   B  - best result so far, A - average of the last 10 results, H - help.
 *
 * Deliberate choices:
 *   - the time base is get_clock_ticks() (core/tick.c, one tick = 1 ms), so the
 *     resolution of the measurement is 1 ms and the value includes the latency
 *     of the terminal and of the UART, not only the human reaction;
 *   - both UART3 and UART5 are polled with a timeout of 0, which the driver
 *     treats as "return immediately" (NS16550_read), so the loop never blocks
 *     and the tick comparison stays accurate; the text is printed on both
 *     ports and the answer goes back to the port the key came from, so the
 *     serial cable may be on either DB9 connector;
 *   - a key pressed after R but before GO is a false start and cancels the
 *     measurement, because otherwise a nervous player could "win" by pressing
 *     early;
 *   - every output line is assembled in a RAM buffer and sent with ONE call to
 *     ls1x_uart_write(). The first version wrote one character per call; in two
 *     of four measurements a stray character appeared inside the printed number
 *     ("34a7" instead of "347", "25=1" instead of "251"), so the per-character
 *     write path was the first suspect and was replaced by a single write per
 *     line;
 *   - no result is kept in flash: the statistics live in RAM and reset on a
 *     board reset, which keeps the program free of flash writes.
 */

#include <stdio.h>
#include "ls1b.h"
#include "mips.h"
#include "ns16550.h"
#include "bsp.h"

#define NUM_PORTS   2           /* UART3 and UART5                          */
#define WINDOW      10          /* how many results the average keeps       */
#define ARM_MIN     1000u       /* shortest wait after R (ms)               */
#define ARM_MAX     3000u       /* longest  wait after R (ms)               */
#define GIVE_UP     5000u       /* no key within this time -> timeout (ms)  */
#define LINE_MAX    320         /* longest output line (help block = 220)   */

/* states of the little machine */
#define ST_IDLE     0
#define ST_ARMED    1
#define ST_CUE      2

static void *port[NUM_PORTS];   /* filled in main() from devUART3/devUART5  */

static int          state      = ST_IDLE;
static unsigned int arm_ticks  = 0;
static unsigned int arm_delay  = 0;
static unsigned int cue_ticks  = 0;

static unsigned int best_ms    = 0;                    /* 0 = no result yet */
static unsigned int win[WINDOW];
static int          win_count  = 0;
static int          win_next   = 0;

/* one output line, assembled here and sent with a single UART write */
static char line_buf[LINE_MAX];
static int  line_len = 0;

/* -------------------------------------------------------------------------- */
/* line builder                                                               */
/* -------------------------------------------------------------------------- */

static void lb_reset(void)
{
    line_len = 0;
}

static void lb_ch(char c)
{
    if (line_len < (LINE_MAX - 1))
    {
        line_buf[line_len++] = c;
    }
}

static void lb_str(const char *s)
{
    while (*s != '\0')
    {
        lb_ch(*s++);
    }
}

static void lb_num(unsigned int v)
{
    char tmp[12];
    int  i = 0;

    if (v == 0u)
    {
        lb_ch('0');
        return;
    }
    while ((v > 0u) && (i < 11))
    {
        tmp[i++] = (char)('0' + (int)(v % 10u));
        v /= 10u;
    }
    while (i > 0)
    {
        lb_ch(tmp[--i]);
    }
}

static void lb_send(void *p)
{
    if (line_len > 0)
    {
        ls1x_uart_write(p, line_buf, line_len, NULL);
    }
}

static void lb_send_all(void)
{
    int i;

    for (i = 0; i < NUM_PORTS; i++)
    {
        lb_send(port[i]);
    }
}

/* -------------------------------------------------------------------------- */
/* statistics                                                                 */
/* -------------------------------------------------------------------------- */

static void record(unsigned int ms)
{
    win[win_next] = ms;
    win_next = (win_next + 1) % WINDOW;
    if (win_count < WINDOW)
    {
        win_count++;
    }
    if ((best_ms == 0u) || (ms < best_ms))
    {
        best_ms = ms;
    }
}

static unsigned int average(void)
{
    unsigned int sum = 0u;
    int i;

    if (win_count == 0)
    {
        return 0u;
    }
    for (i = 0; i < win_count; i++)
    {
        sum += win[i];
    }
    return sum / (unsigned int)win_count;
}

/* -------------------------------------------------------------------------- */
/* the random wait: two reads of the millisecond counter, mixed together      */
/* -------------------------------------------------------------------------- */

static unsigned int pick_delay(void)
{
    unsigned int a = get_clock_ticks();
    unsigned int b = get_clock_ticks() << 5;
    unsigned int s = a ^ b;

    return ARM_MIN + (s % (ARM_MAX - ARM_MIN + 1u));
}

/* -------------------------------------------------------------------------- */
/* one key from one port                                                      */
/* -------------------------------------------------------------------------- */

static void handle(void *p, char c, unsigned int now)
{
    char up = ((c >= 'a') && (c <= 'z')) ? (char)(c - 32) : c;
    unsigned int ms;

    ls1x_uart_write(p, &c, 1, NULL);            /* echo the key             */

    if (state == ST_ARMED)
    {
        lb_reset();
        lb_str("\r\n  FALSE START - the cue had not come yet.\r\n"
               "  type R for the next try\r\n> ");
        lb_send(p);
        state = ST_IDLE;
        return;
    }

    if (state == ST_CUE)
    {
        ms = now - cue_ticks;
        record(ms);
        lb_reset();
        lb_str("\r\n  reaction: ");
        lb_num(ms);
        lb_str(" ms\r\n  best: ");
        lb_num(best_ms);
        lb_str(" ms   average of ");
        lb_num((unsigned int)win_count);
        lb_str(": ");
        lb_num(average());
        lb_str(" ms\r\n  R = next try, B = best, A = average, H = help\r\n> ");
        lb_send(p);
        state = ST_IDLE;
        return;
    }

    /* idle */
    lb_reset();
    switch (up)
    {
    case 'R':
        arm_delay = pick_delay();
        arm_ticks = now;
        state     = ST_ARMED;
        lb_str("\r\n  get ready ...\r\n");
        lb_send(p);
        return;

    case 'B':
        if (best_ms == 0u)
        {
            lb_str("\r\n  best: no result yet\r\n> ");
        }
        else
        {
            lb_str("\r\n  best: ");
            lb_num(best_ms);
            lb_str(" ms\r\n> ");
        }
        lb_send(p);
        return;

    case 'A':
        if (win_count == 0)
        {
            lb_str("\r\n  average: no result yet\r\n> ");
        }
        else
        {
            lb_str("\r\n  average of ");
            lb_num((unsigned int)win_count);
            lb_str(": ");
            lb_num(average());
            lb_str(" ms\r\n> ");
        }
        lb_send(p);
        return;

    case 'H':
        lb_str("\r\n  commands:\r\n"
               "    R  - get ready and wait for the cue\r\n"
               "    B  - best result so far\r\n"
               "    A  - average of the last 10 results\r\n"
               "    H  - this list\r\n"
               "  any other key: start a measurement (R), or answer the cue (after GO)\r\n> ");
        lb_send(p);
        return;

    default:
        lb_str("\r\n  ?  type R to start a measurement, H for help\r\n> ");
        lb_send(p);
        return;
    }
}

/* -------------------------------------------------------------------------- */

int main(void)
{
    char c;
    int  i;
    unsigned int now;

    ls1x_drv_init();
    install_3th_libraries();

    port[0] = devUART3;
    port[1] = devUART5;

    for (i = 0; i < NUM_PORTS; i++)
    {
        ls1x_uart_init(port[i], NULL);
        ls1x_uart_open(port[i], NULL);
    }

    lb_reset();
    lb_str("\r\n\r\n=== Loongson 1B : reflex test ===\r\n"
           "  R = ready, B = best, A = average, H = help\r\n"
           "  1 tick = 1 ms; the value includes the terminal latency\r\n> ");
    lb_send_all();

    for (;;)
    {
        now = get_clock_ticks();

        for (i = 0; i < NUM_PORTS; i++)
        {
            /* timeout 0 => the driver returns immediately, the loop never blocks */
            if (ls1x_uart_read(port[i], &c, 1, 0) > 0)
            {
                handle(port[i], c, now);
            }
        }

        if ((state == ST_ARMED) && ((now - arm_ticks) >= arm_delay))
        {
            cue_ticks = get_clock_ticks();
            state     = ST_CUE;
            lb_reset();
            lb_str("\r\n\r\n  >>>> GO! <<<<\r\n");
            lb_send_all();
        }
        else if ((state == ST_CUE) && ((now - cue_ticks) > GIVE_UP))
        {
            state = ST_IDLE;
            lb_reset();
            lb_str("\r\n  timeout - no key within 5 s\r\n> ");
            lb_send_all();
        }
    }

    return 0;
}
