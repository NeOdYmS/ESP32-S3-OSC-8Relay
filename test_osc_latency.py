#!/usr/bin/env python3
"""
OSC Latency Test for ESP32-S3-OSC-8Relay
Pure UDP timing - no REST API needed.
Works over Ethernet (192.168.0.1:8000)
"""

import time
import socket
import struct
import subprocess

ESP32_IP = "192.168.0.1"
OSC_PORT = 8000


def osc_encode(addr, value):
    """Encode OSC message with int32 argument."""
    addr_bytes = addr.encode() + b'\x00'
    while len(addr_bytes) % 4 != 0:
        addr_bytes += b'\x00'
    tag_bytes = b',i\x00\x00'
    data = struct.pack('>i', value)
    return addr_bytes + tag_bytes + data


def main():
    print("=" * 60)
    print("  OSC Latency Test - ESP32-S3-OSC-8Relay")
    print("  Target: %s:%d" % (ESP32_IP, OSC_PORT))
    print("=" * 60)

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.settimeout(0.1)
    dest = (ESP32_IP, OSC_PORT)

    # ========== TEST 1: UDP send latency ==========
    print("\n" + "=" * 60)
    print("  TEST 1: UDP send latency (20 messages)")
    print("=" * 60)

    send_times = []
    for i in range(20):
        relay = (i % 8) + 1
        state = i % 2
        msg = osc_encode("/relay/%d" % relay, state)
        t0 = time.perf_counter()
        sock.sendto(msg, dest)
        t1 = time.perf_counter()
        us = (t1 - t0) * 1000000
        send_times.append(us)
        time.sleep(0.05)

    print("   Min:  %.0f us" % min(send_times))
    print("   Max:  %.0f us" % max(send_times))
    print("   Avg:  %.0f us" % (sum(send_times) / len(send_times)))

    # ========== TEST 2: Rapid toggle relay 1 ==========
    print("\n" + "=" * 60)
    print("  TEST 2: Rapid toggle relay 1 (50 ON/OFF cycles)")
    print("=" * 60)

    sock.sendto(osc_encode("/relay/all", 0), dest)
    time.sleep(0.3)

    toggle_times = []
    for i in range(50):
        msg = osc_encode("/relay/1", i % 2)
        t0 = time.perf_counter()
        sock.sendto(msg, dest)
        t1 = time.perf_counter()
        toggle_times.append((t1 - t0) * 1000000)
        time.sleep(0.01)

    print("   Min:  %.0f us" % min(toggle_times))
    print("   Max:  %.0f us" % max(toggle_times))
    print("   Avg:  %.0f us" % (sum(toggle_times) / len(toggle_times)))
    print("   50 cycles en %.1f ms total send" % (sum(toggle_times) / 1000))

    # ========== TEST 3: Burst 8 relays ==========
    print("\n" + "=" * 60)
    print("  TEST 3: Burst 8 relais ON puis OFF")
    print("=" * 60)

    sock.sendto(osc_encode("/relay/all", 0), dest)
    time.sleep(0.3)

    t0 = time.perf_counter()
    for r in range(1, 9):
        sock.sendto(osc_encode("/relay/%d" % r, 1), dest)
    t1 = time.perf_counter()
    burst_on_ms = (t1 - t0) * 1000
    print("   8x ON  en: %.2f ms (%.0f us/msg)" % (burst_on_ms, burst_on_ms * 1000 / 8))

    time.sleep(0.3)

    t0 = time.perf_counter()
    for r in range(1, 9):
        sock.sendto(osc_encode("/relay/%d" % r, 0), dest)
    t1 = time.perf_counter()
    burst_off_ms = (t1 - t0) * 1000
    print("   8x OFF en: %.2f ms (%.0f us/msg)" % (burst_off_ms, burst_off_ms * 1000 / 8))

    # ========== TEST 4: /relay/all ==========
    print("\n" + "=" * 60)
    print("  TEST 4: /relay/all ON puis OFF (10 cycles)")
    print("=" * 60)

    all_times = []
    for i in range(10):
        msg = osc_encode("/relay/all", i % 2)
        t0 = time.perf_counter()
        sock.sendto(msg, dest)
        t1 = time.perf_counter()
        all_times.append((t1 - t0) * 1000000)
        time.sleep(0.1)

    print("   Min:  %.0f us" % min(all_times))
    print("   Max:  %.0f us" % max(all_times))
    print("   Avg:  %.0f us" % (sum(all_times) / len(all_times)))

    # ========== TEST 5: Throughput ==========
    print("\n" + "=" * 60)
    print("  TEST 5: Throughput - 1000 messages sans pause")
    print("=" * 60)

    messages = [osc_encode("/relay/%d" % ((i % 8) + 1), i % 2) for i in range(1000)]

    t0 = time.perf_counter()
    for msg in messages:
        sock.sendto(msg, dest)
    t1 = time.perf_counter()

    total_ms = (t1 - t0) * 1000
    rate = 1000 / (t1 - t0)
    print("   1000 messages en %.1f ms" % total_ms)
    print("   Debit: %.0f msg/s" % rate)
    print("   Latence/msg: %.0f us" % (total_ms * 1000 / 1000))

    # ========== TEST 6: ICMP ping reference ==========
    print("\n" + "=" * 60)
    print("  TEST 6: ICMP ping (reference reseau)")
    print("=" * 60)

    try:
        result = subprocess.run(
            ["ping", "-c", "5", "-i", "0.2", ESP32_IP],
            capture_output=True, text=True, timeout=10
        )
        for line in result.stdout.split('\n'):
            if 'round-trip' in line or 'rtt' in line or 'avg' in line:
                print("   %s" % line.strip())
                break
    except Exception:
        print("   ping indisponible")

    # Cleanup
    sock.sendto(osc_encode("/relay/all", 0), dest)
    time.sleep(0.2)
    sock.close()

    # ========== RESUME ==========
    avg_send = sum(send_times) / len(send_times)
    avg_toggle = sum(toggle_times) / len(toggle_times)
    print("\n" + "=" * 60)
    print("  RESUME")
    print("=" * 60)
    print("  UDP send latency:     ~%.0f us" % avg_send)
    print("  Toggle rate:          ~%.0f msg/s theorique" % (1000000 / avg_toggle))
    print("  Burst 8 relais:       ~%.2f ms" % ((burst_on_ms + burst_off_ms) / 2))
    print("  Throughput max:       ~%.0f msg/s" % rate)
    print("  Latence totale estimee OSC->relais:")
    print("    UDP reseau W5500:   ~0.3-0.7 ms (ping)")
    print("    ESP32 parsing OSC:  ~0.01-0.05 ms")
    print("    I2C PCA9554 write:  ~0.2-0.5 ms (100kHz)")
    print("    = Total estime:     ~0.5-1.2 ms")
    print()


if __name__ == "__main__":
    main()
