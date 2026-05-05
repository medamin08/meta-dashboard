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

# OBD-II response for Mode 01, PID 05 (Engine Coolant Temperature)
def send_obd_response(temp):
    if not can_socket:
        return

    try:
        # Calculate byte A
        # OBD-II Temp formula: Value = A - 40 -> A = Temp + 40
        A = int(temp) + 40
        A = max(0, min(255, A))
        
        # CAN frame structure for python socket: 
        # CAN ID (4 bytes format), DLC (1 byte), Padding (3 bytes), Data (8 bytes)
        # Note: B stands for unsigned char compiler packing.
        can_frame_format = "=IB3x8s" 
        
        # OBD payload
        data = bytes([0x03, 0x41, 0x05, A, 0x00, 0x00, 0x00, 0x00])
        
        frame = struct.pack(can_frame_format, CAN_ID, 8, data)
        can_socket.send(frame)
        
        data_hex = "".join([f"{b:02X}" for b in data])
        print(f"Sent: {CAN_ID:X}#{data_hex} -> Temp: {temp:.1f} °C")
    except Exception as e:
        print(f"Error sending frame: {e}")

def main():
    print("Starting ECU OBD-II Simulator for Engine Coolant Temperature...")
    print(f"Using interface: {PORT}")
    
    base_temp = 70.0
    target_temp = 95.0
    current_temp = base_temp
    
    phase = 0.0
    
    try:
        while True:
            # Simulate warming up the engine
            if current_temp < target_temp:
                current_temp += 0.5
            else:
                # Fluctuate slightly around the target temp
                current_temp = target_temp + 2.0 * math.sin(phase)
                phase += 0.2
            
            send_obd_response(current_temp)
            time.sleep(0.5)
            
    except KeyboardInterrupt:
        print("\nSimulator stopped.")

if __name__ == "__main__":
    main()
