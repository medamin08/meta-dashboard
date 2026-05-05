import os
import time
import struct
import socket
import select
import random

PORT = "vcan0"
CAN_ID = 0x7DF
RESP_ID = 0x7E8

AVAILABLE_DTCS = [
    (0x01, 0x13, "P0113"),
    (0x03, 0x00, "P0300"),
    (0x03, 0x40, "P0340"),
    (0x00, 0x16, "P0016"),
    (0xC1, 0x55, "U0155"),
    (0x05, 0x07, "P0507"),
    (0xAB, 0x00, "B2B00"),
    (0x40, 0x35, "C0035"),
    (0xC1, 0x00, "U0100"),
]

active_dtcs = []
last_dtc_change_time = 0

def randomize_dtcs():
    global active_dtcs, last_dtc_change_time
    num_dtcs = random.randint(1, 2)
    active_dtcs = random.sample(AVAILABLE_DTCS, num_dtcs)
    last_dtc_change_time = time.time()

def setup_can_socket(interface):
    try:
        s = socket.socket(socket.PF_CAN, socket.SOCK_RAW, socket.CAN_RAW)
        s.bind((interface,))
        s.setblocking(False)
        return s
    except Exception as e:
        print(f"Failed to bind to {interface}: {e}")
        return None

can_socket = setup_can_socket(PORT)

def send_dtc_response():
    if not can_socket:
        return
    try:
        can_frame_format = "=IB3x8s" 
        count = len(active_dtcs)
        
        if count > 2:
            count = 2
            
        length = 2 + 2 * count
        payload = [length, 0x43, count]
        
        dtc_names = []
        for i in range(count):
            dtc = active_dtcs[i]
            payload.extend([dtc[0], dtc[1]])
            dtc_names.append(dtc[2])
            
        while len(payload) < 8:
            payload.append(0x00)
            
        data = bytes(payload)
        frame = struct.pack(can_frame_format, RESP_ID, 8, data)
        can_socket.send(frame)
        print(f"Sent DTC Response: {', '.join(dtc_names) if count > 0 else 'No DTCs'}")
    except Exception as e:
        print(f"Error sending frame: {e}")

def send_clear_response():
    if not can_socket:
        return
    try:
        can_frame_format = "=IB3x8s" 
        data = bytes([0x01, 0x44, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00])
        frame = struct.pack(can_frame_format, RESP_ID, 8, data)
        can_socket.send(frame)
        print("Sent Clear DTC Response")
    except Exception as e:
        print(f"Error sending frame: {e}")

def main():
    print("Starting ECU OBD-II Simulator for DTCs...")
    global active_dtcs, last_dtc_change_time
    last_send = time.time()
    randomize_dtcs()
    
    try:
        while True:
            ready = select.select([can_socket], [], [], 0.5)
            if ready[0]:
                frame, _ = can_socket.recvfrom(16)
                can_id, can_dlc, data = struct.unpack("=IB3x8s", frame)
                if can_id == CAN_ID:
                    length = data[0]
                    mode = data[1]
                    if mode == 0x03:
                        print("Received Mode 03 (Request DTCs)")
                        time.sleep(0.1)
                        randomize_dtcs()
                        send_dtc_response()
                        last_send = time.time()
                    elif mode == 0x04:
                        print("Received Mode 04 (Clear DTCs)")
                        time.sleep(0.1)
                        active_dtcs = []
                        send_clear_response()
                        last_send = time.time()
            
            # Broadcast DTCs periodically to ensure the dashboard receives them
            # even if it missed the initial request.
            if time.time() - last_send > 1.0:
                send_dtc_response()
                last_send = time.time()
    except KeyboardInterrupt:
        print("\nDTC Simulator stopped.")

if __name__ == "__main__":
    main()
