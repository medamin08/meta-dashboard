#!/usr/bin/env python3
import time
import math
import struct
import socket

PORT   = "vcan0"
CAN_ID = 0x7E8

def setup_can_socket(interface):
    try:
        s = socket.socket(socket.PF_CAN, socket.SOCK_RAW, socket.CAN_RAW)
        s.bind((interface,))
        return s
    except Exception as e:
        print(f"Failed to bind to {interface}: {e}")
        return None

can_socket = setup_can_socket(PORT)

# OBD-II response for Mode 01, PID 6D (Brake Pedal/Fluid Pressure)
# Encoding: pressure_kPa = (256*A + B) * 0.03
# Bar = kPa / 100
def send_brake_response(pressure_bar):
    if not can_socket:
        return

    try:
        kPa = pressure_bar * 100.0
        raw = int(kPa / 0.03)
        raw = max(0, min(65535, raw))
        A = raw // 256
        B = raw % 256

        can_frame_format = "=IB3x8s"
        data = bytes([0x04, 0x41, 0x6D, A, B, 0x00, 0x00, 0x00])
        frame = struct.pack(can_frame_format, CAN_ID, 8, data)
        can_socket.send(frame)

        data_hex = "".join([f"{b:02X}" for b in data])
        print(f"Sent: {CAN_ID:X}#{data_hex} -> Brake Pressure: {pressure_bar:.2f} bar")
    except Exception as e:
        print(f"Error sending frame: {e}")


def main():
    print("Starting ECU OBD-II Simulator for Brake Fluid Pressure...")
    print(f"Using interface: {PORT}")

    t = 0.0
    try:
        while True:
            # Simulate periodic braking events:
            # idle ~0 bar, with sharp peaks up to ~80 bar every few seconds
            # Use a rectified sine to model pedal-press shape
            cycle  = math.sin(t * 0.4)           # slow cycle
            pedal  = max(0.0, cycle) ** 2         # only positive phase, squared for sharpness
            pressure_bar = pedal * 80.0           # max 80 bar

            send_brake_response(pressure_bar)
            time.sleep(0.1)
            t += 0.1

    except KeyboardInterrupt:
        print("\nBrake Pressure Simulator stopped.")


if __name__ == "__main__":
    main()
