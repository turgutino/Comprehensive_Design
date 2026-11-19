import struct, sys
sys.stdout.reconfigure(encoding="utf-8")
path = r"E:\Projects\Compherisive design\evidence\example_8_lwip\udp_client_capture.pcapng"
data = open(path, "rb").read()
print("file size", len(data))

def csum(b):
    if len(b) % 2: b += b"\x00"
    s = sum(struct.unpack("!%dH" % (len(b) // 2), b))
    while s >> 16: s = (s & 0xFFFF) + (s >> 16)
    return (~s) & 0xFFFF

pos = 0
pkts = []
while pos + 12 <= len(data):
    btype, blen = struct.unpack_from("<II", data, pos)
    if blen < 12: break
    body = data[pos + 8: pos + blen - 4]
    if btype == 6:  # Enhanced Packet Block
        iface, ts_hi, ts_lo, caplen, origlen = struct.unpack_from("<IIIII", body, 0)
        pkts.append(body[20:20 + caplen])
    elif btype == 3:  # Simple Packet Block
        origlen, = struct.unpack_from("<I", body, 0)
        pkts.append(body[4:4 + origlen])
    pos += blen
print("packets in pcapng:", len(pkts))

seen = set()
for n, p in enumerate(pkts, 1):
    if len(p) < 14: continue
    et = struct.unpack_from("!H", p, 12)[0]
    if et == 0x0806:
        continue
    if et != 0x0800:
        continue
    ip = p[14:]
    ihl = (ip[0] & 15) * 4
    proto = ip[9]
    src = ".".join(map(str, ip[12:16])); dst = ".".join(map(str, ip[16:20]))
    if proto != 17:
        print("pkt %d: IPv4 proto %d %s>%s" % (n, proto, src, dst)); continue
    udp = ip[ihl:]
    sport, dport, ulen, ucs = struct.unpack_from("!HHHH", udp, 0)
    payload = udp[8:ulen]
    key = (src, dst, sport, dport, bytes(payload), ucs)
    # IP header checksum: must be 0 when summed including the checksum field
    ip_ok = csum(ip[:ihl]) == 0
    # UDP checksum over pseudo header
    pseudo = ip[12:16] + ip[16:20] + b"\x00" + bytes([17]) + struct.pack("!H", ulen)
    if ucs == 0:
        uok = "checksum field is ZERO (sender did not compute it)"
    else:
        uok = "valid" if csum(pseudo + udp[:ulen]) == 0 else "INVALID (computed %04x, field %04x)" % (csum(pseudo + udp[:8].replace(udp[6:8], b"\x00\x00") + payload), ucs)
    tag = "dup" if key in seen else "new"
    seen.add(key)
    print("pkt %d [%s]: %s:%d > %s:%d  UDP len=%d  IPhdrChecksum=%s  UDPchecksum=%s  ip.id=0x%04x ttl=%d frag/flags=0x%04x" % (
        n, tag, src, sport, dst, dport, ulen, "valid" if ip_ok else "INVALID", uok,
        struct.unpack_from("!H", ip, 4)[0], ip[8], struct.unpack_from("!H", ip, 6)[0]))
    if tag == "new":
        print("   payload:", payload, "| dst MAC", p[0:6].hex("-"), "src MAC", p[6:12].hex("-"), "| frame len", len(p))
        print("   ip header:", ip[:ihl].hex(" "))
        print("   udp header:", udp[:8].hex(" "))
