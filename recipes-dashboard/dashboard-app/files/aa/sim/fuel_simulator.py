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

# OBD-II response for Mode 01, PID 2F (Fuel Tank Level Input)
def send_fuel_response(fuel_pct):
    if not can_socket:
        return

    try:
        # Calculate byte A
        # OBD-II Fuel formula: Value = A * 100 / 255 -> A = Value * 255 / 100
        A = int(fuel_pct * 255 / 100)
        A = max(0, min(255, A))
        
        # CAN frame structure for python socket: 
        can_frame_format = "=IB3x8s" 
        
        # OBD payload
        # 0x03 (Length), 0x41 (Mode 1), 0x2F (PID for Fuel), A (Data)
        data = bytes([0x03, 0x41, 0x2F, A, 0x00, 0x00, 0x00, 0x00])
        
        frame = struct.pack(can_frame_format, CAN_ID, 8, data)
        can_socket.send(frame)
        
        data_hex = "".join([f"{b:02X}" for b in data])
        print(f"Sent: {CAN_ID:X}#{data_hex} -> Fuel: {fuel_pct:.1f} %")
    except Exception as e:
        print(f"Error sending frame: {e}")

def main():
    print("Starting ECU OBD-II Simulator for Fuel Level...")
    print(f"Using interface: {PORT}")
    
    current_fuel = 100.0  # Start at 100%
    target_fuel = 5.0     # Decrease to 5%
    
    try:
        while True:
            # Simulate draining fuel fairly quickly so we can see the 10% blink
            if current_fuel > target_fuel:
                current_fuel -= 1.5
            
            send_fuel_response(current_fuel)
            time.sleep(0.5)
            
    except KeyboardInterrupt:
        print("\nFuel Simulator stopped.")

if __name__ == "__main__":
    main()
