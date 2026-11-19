#!/usr/bin/env python3
"""PC-side peer for the LwIP tests of guide section 8 (replaces NetAssist).

Board: 192.168.1.123   PC: 192.168.1.111
Ports (lwip_test.h): TCP board-server 9060, TCP PC-server 9061,
                     UDP board-server 9062, UDP PC-server 9063.

Modes (one per guide test):
  tcp-client   PC connects to the board's TCP server (guide 8.2.12, TEST_TCP_SERVER)
  tcp-server   PC listens, the board connects (guide 8.2.13, TEST_TCP_CLIENT)
  udp-client   PC sends datagrams to the board's UDP server (guide 8.2.14, TEST_UDP_SERVER)
  udp-server   PC listens, the board sends datagrams (guide 8.2.15, TEST_UDP_CLIENT)
  selftest     run all four modes against local echo stand-ins on 127.0.0.1

Every event is printed with a timestamp and also appended to --log.
"""
import argparse
import socket
import sys
import threading
import time

BOARD_IP = "192.168.1.123"
PC_IP = "192.168.1.111"


class Log:
    def __init__(self, path):
        self.f = open(path, "a", encoding="utf-8") if path else None

    def __call__(self, msg):
        line = "%s  %s" % (time.strftime("%H:%M:%S"), msg)
        print(line, flush=True)
        if self.f:
            self.f.write(line + "\n")
            self.f.flush()


def tcp_client(log, host, port, count, wait):
    """Board is the TCP server: connect, send `count` messages, print every echo."""
    log("tcp-client: connecting to %s:%d" % (host, port))
    s = socket.create_connection((host, port), timeout=wait)
    log("tcp-client: connected, local %s:%d" % s.getsockname())
    for i in range(1, count + 1):
        msg = "hello from PC %d\n" % i
        s.sendall(msg.encode())
        log("SEND    %r" % msg)
        try:
            data = s.recv(256)
        except socket.timeout:
            log("RECV    (timeout after %.0f s, no echo)" % wait)
            continue
        if not data:
            log("RECV    connection closed by the board")
            break
        log("RECV    %r" % data.decode(errors="replace"))
        time.sleep(0.3)
    s.close()
    log("tcp-client: done")


def tcp_server(log, bind, port, wait, replies):
    """Board is the TCP client: accept, answer every message (the board waits for it)."""
    srv = socket.socket()
    srv.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    srv.bind((bind, port))
    srv.listen(1)
    srv.settimeout(wait)
    log("tcp-server: listening on %s:%d (waiting %.0f s for the board)" % (bind, port, wait))
    conn, addr = srv.accept()
    log("tcp-server: board connected from %s:%d" % addr)
    conn.settimeout(wait)
    n = 0
    while True:
        try:
            data = conn.recv(256)
        except socket.timeout:
            log("tcp-server: no data for %.0f s, stopping" % wait)
            break
        if not data:
            log("tcp-server: board closed the connection")
            break
        n += 1
        log("RECV    %r" % data.decode(errors="replace"))
        if n <= replies:
            reply = "reply %d from PC\n" % n
            conn.sendall(reply.encode())
            log("SEND    %r" % reply)
    conn.close()
    srv.close()
    log("tcp-server: done, %d message(s) received" % n)


def udp_client(log, host, port, local_port, count, wait):
    """Board is the UDP server (echo): send datagrams from PC port `local_port`."""
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.bind((PC_IP if host == BOARD_IP else "127.0.0.1", local_port))
    s.settimeout(wait)
    log("udp-client: %s -> %s:%d" % (s.getsockname(), host, port))
    for i in range(1, count + 1):
        msg = "udp hello from PC %d" % i
        s.sendto(msg.encode(), (host, port))
        log("SEND    %r" % msg)
        try:
            data, addr = s.recvfrom(256)
            log("RECV    %r from %s:%d" % (data.decode(errors="replace"), addr[0], addr[1]))
        except socket.timeout:
            log("RECV    (timeout after %.0f s, no echo)" % wait)
    s.close()
    log("udp-client: done")


def udp_server(log, bind, port, wait, replies, nudge=0):
    """Board is the UDP client: answer every datagram (the board blocks until one arrives).

    nudge > 0: while no datagram has arrived yet, send a short datagram to the board's first
    ephemeral port (49153) every `nudge` seconds. On 2026-11-10 the first datagram the board
    sent after a boot never reached the PC application (dropped in the PC receive path, see
    REPORT.md 5.4) and the board then waits forever in recvfrom(); the nudge makes it send again.
    """
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    s.bind((bind, port))
    s.settimeout(wait)
    log("udp-server: listening on %s:%d (waiting %.0f s)" % (bind, port, wait))
    n = 0
    waited = 0.0
    while True:
        s.settimeout(nudge if (nudge and n == 0) else wait)
        try:
            data, addr = s.recvfrom(256)
        except socket.timeout:
            if nudge and n == 0:
                waited += nudge
                if waited < wait:
                    s.sendto(b"nudge", (BOARD_IP, 49153))
                    log("NUDGE   sent to %s:49153 (no datagram from the board yet)" % BOARD_IP)
                    continue
            log("udp-server: no datagram for %.0f s, stopping" % wait)
            break
        n += 1
        log("RECV    %r from %s:%d" % (data.decode(errors="replace"), addr[0], addr[1]))
        if n <= replies:
            reply = "reply %d from PC" % n
            s.sendto(reply.encode(), addr)
            log("SEND    %r to %s:%d" % (reply, addr[0], addr[1]))
    s.close()
    log("udp-server: done, %d datagram(s) received" % n)


def selftest(log):
    """Loopback check of the four modes with simple stand-ins for the board."""
    ok = True
    # tcp-client against an echo server
    srv = socket.socket(); srv.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    srv.bind(("127.0.0.1", 19060)); srv.listen(1)
    def echo():
        c, _ = srv.accept()
        while True:
            d = c.recv(256)
            if not d: break
            c.sendall(d)
        c.close()
    threading.Thread(target=echo, daemon=True).start()
    tcp_client(log, "127.0.0.1", 19060, 2, 2)
    srv.close()
    # tcp-server against a client that sends 3 lines and reads the replies
    def client():
        time.sleep(0.3)
        c = socket.create_connection(("127.0.0.1", 19061))
        for i in range(3):
            c.sendall(("client ticks = %d.\n" % i).encode()); c.recv(256); time.sleep(0.1)
        c.close()
    threading.Thread(target=client, daemon=True).start()
    tcp_server(log, "127.0.0.1", 19061, 3, 5)
    # udp-client against an echo
    e = socket.socket(socket.AF_INET, socket.SOCK_DGRAM); e.bind(("127.0.0.1", 19062))
    def uecho():
        for _ in range(2):
            d, a = e.recvfrom(256); e.sendto(d, a)
    threading.Thread(target=uecho, daemon=True).start()
    udp_client(log, "127.0.0.1", 19062, 19072, 2, 2)
    e.close()
    # udp-server against a sender that waits for the reply
    def usender():
        time.sleep(0.3)
        c = socket.socket(socket.AF_INET, socket.SOCK_DGRAM); c.settimeout(2)
        for i in range(3):
            c.sendto(("client ticks = %d." % i).encode(), ("127.0.0.1", 19063)); c.recvfrom(256)
    threading.Thread(target=usender, daemon=True).start()
    udp_server(log, "127.0.0.1", 19063, 3, 5)
    log("selftest: finished")
    return ok


def main():
    p = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    p.add_argument("mode", choices=["tcp-client", "tcp-server", "udp-client", "udp-server", "selftest"])
    p.add_argument("--board", default=BOARD_IP)
    p.add_argument("--bind", default=PC_IP)
    p.add_argument("--count", type=int, default=5, help="messages to send (client modes)")
    p.add_argument("--wait", type=float, default=30, help="seconds to wait for the board / a reply")
    p.add_argument("--replies", type=int, default=5, help="replies to send (server modes)")
    p.add_argument("--nudge", type=float, default=0,
                   help="udp-server: seconds between nudge datagrams to the board while nothing has arrived")
    p.add_argument("--log", default="")
    a = p.parse_args()
    log = Log(a.log)
    log("mode=%s" % a.mode)
    if a.mode == "tcp-client":
        tcp_client(log, a.board, 9060, a.count, a.wait)
    elif a.mode == "tcp-server":
        tcp_server(log, a.bind, 9061, a.wait, a.replies)
    elif a.mode == "udp-client":
        udp_client(log, a.board, 9062, 9062, a.count, a.wait)
    elif a.mode == "udp-server":
        udp_server(log, a.bind, 9063, a.wait, a.replies, a.nudge)
    else:
        selftest(log)


if __name__ == "__main__":
    sys.exit(main())
