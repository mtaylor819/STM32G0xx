#!/usr/bin/env python3
"""Spindle PWM test for grblHAL - cycles duty on the TH0 pin (PA0) until Ctrl-C.

Usage: python3 tools/spindle_test.py [port]
Close any g-code sender first (the serial port must be free).
"""

import glob
import os
import sys
import termios
import time

port = sys.argv[1] if len(sys.argv) > 1 else (glob.glob('/dev/cu.usbmodem*') or [None])[0]
if not port:
    sys.exit("No /dev/cu.usbmodem* found - is the board connected via USB?")

fd = os.open(port, os.O_RDWR | os.O_NOCTTY | os.O_NONBLOCK)
a = termios.tcgetattr(fd)
a[0] = a[1] = a[3] = 0
a[4] = a[5] = termios.B115200
termios.tcsetattr(fd, termios.TCSANOW, a)
time.sleep(1)


def send(cmd):
    os.write(fd, cmd + b'\r')
    time.sleep(0.3)
    try:
        while os.read(fd, 4096):
            pass
    except BlockingIOError:
        pass


send(b'')
send(b'$X')

# Read $30 (max rpm) so the steps are true percentages of full scale
os.write(fd, b'$$\r')
time.sleep(1.5)
out = b''
try:
    while True:
        c = os.read(fd, 4096)
        if not c:
            break
        out += c
except BlockingIOError:
    pass
max_rpm = 1000
for line in out.decode(errors='replace').splitlines():
    if line.startswith('$30='):
        max_rpm = float(line.split('=')[1].split()[0])
print(f"Conectado a {port}. $30={max_rpm:.0f} rpm. Ciclando PWM en PA1 (SERVOS). Ctrl-C para salir.")

def rpm(pct):
    return f'M3S{max_rpm * pct / 100:.0f}'.encode()

STEPS = [(rpm(100), '100% duty (3.3V fijo)'),
         (rpm(75), ' 75% duty'),
         (rpm(50), ' 50% duty'),
         (rpm(25), ' 25% duty'),
         (b'M5', '  0% (0V, spindle off)')]

try:
    while True:
        for cmd, desc in STEPS:
            print(f"  {cmd.decode():<9} -> {desc}")
            send(cmd)
            time.sleep(6)
except KeyboardInterrupt:
    print("\nParando spindle...")
    send(b'M5')
    os.close(fd)
