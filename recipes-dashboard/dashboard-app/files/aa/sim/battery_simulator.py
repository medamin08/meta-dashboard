#!/usr/bin/env python3
import os
import time
import math
import struct
import socket

PORT = "vcan0"
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

# OBD-II response for Mode 01, PID 42 (Control Module Voltage)
def send_battery_response(voltage):
    if not can_socket:
        return

    try:
        # Calculate byte A and B
        # OBD-II Voltage formula: Value = (256 * A + B) / 1000
        val = int(voltage * 1000)
        A = val // 256
        B = val % 256
        A = max(0, min(255, A))
        B = max(0, min(255, B))
        
        # CAN frame structure for python socket: 
        can_frame_format = "=IB3x8s" 
        
        # OBD payload
        # 4 valid bytes
        data = bytes([0x04, 0x41, 0x42, A, B, 0x00, 0x00, 0x00])
        
        frame = struct.pack(can_frame_format, CAN_ID, 8, data)
        can_socket.send(frame)
        
        data_hex = "".join([f"{b:02X}" for b in data])
        print(f"Sent: {CAN_ID:X}#{data_hex} -> Voltage: {voltage:.2f} V")
    except Exception as e:
        print(f"Error sending frame: {e}")

def main():
    print("Starting ECU OBD-II Simulator for Battery Voltage...")
    print(f"Using interface: {PORT}")
    
    base_voltage = 13.5
    phase = 0.0
    
    try:
        while True:
            # Fluctuate slightly to simulate running alternator / electrical load
            current_voltage = base_voltage + 0.5 * math.sin(phase * 1.5)
            phase += 0.2
            
            send_battery_response(current_voltage)
            time.sleep(0.5)
            
    except KeyboardInterrupt:
        print("\nBattery Simulator stopped.")

if __name__ == "__main__":
    main()
