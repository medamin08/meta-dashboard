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

def send_tire_response(fl, fr, rl, rr):
    if not can_socket:
        return

    try:
        # Values in bar multiplied by 100 to send as integer (0-255)
        fl_val = int(fl * 100)
        fr_val = int(fr * 100)
        rl_val = int(rl * 100)
        rr_val = int(rr * 100)
        
        fl_val = max(0, min(255, fl_val))
        fr_val = max(0, min(255, fr_val))
        rl_val = max(0, min(255, rl_val))
        rr_val = max(0, min(255, rr_val))
        
        # CAN frame structure for python socket
        can_frame_format = "=IB3x8s" 
        
        # OBD payload
        # 6 valid bytes: Length=6, Mode=0x41, PID=0x50, Data=[FL, FR, RL, RR]
        data = bytes([0x06, 0x41, 0x50, fl_val, fr_val, rl_val, rr_val, 0x00])
        
        frame = struct.pack(can_frame_format, CAN_ID, 8, data)
        can_socket.send(frame)
        
        data_hex = "".join([f"{b:02X}" for b in data])
        print(f"Sent: {CAN_ID:X}#{data_hex} -> FL:{fl:.2f} FR:{fr:.2f} RL:{rl:.2f} RR:{rr:.2f} bar")
    except Exception as e:
        print(f"Error sending frame: {e}")

def main():
    print("Starting ECU OBD-II Simulator for Tire Pressures...")
    print(f"Using interface: {PORT}")
    
    base_fl, base_fr = 2.3, 2.3
    base_rl, base_rr = 2.2, 2.2
    phase = 0.0
    
    try:
        while True:
            # Fluctuate slightly
            current_fl = base_fl + 0.05 * math.sin(phase * 0.2)
            current_fr = base_fr + 0.05 * math.cos(phase * 0.2)
            current_rl = base_rl + 0.05 * math.sin(phase * 0.3)
            current_rr = base_rr + 0.05 * math.cos(phase * 0.3)
            phase += 0.2
            
            send_tire_response(current_fl, current_fr, current_rl, current_rr)
            time.sleep(0.5)
            
    except KeyboardInterrupt:
        print("\nTire Simulator stopped.")

if __name__ == "__main__":
    main()
