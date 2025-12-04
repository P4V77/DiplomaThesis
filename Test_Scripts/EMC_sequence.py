import serial
import time
from serial.tools import list_ports


def find_serial_ports():
    """Find all available serial ports."""
    ports = list_ports.comports()
    return [port.device for port in ports]


def main():
    # Automatically find available USB serial ports
    available_ports = find_serial_ports()

    if not available_ports:
        print("No USB serial devices found. Please connect a device and try again.")
        return

    print("Available serial ports:")
    for i, port in enumerate(available_ports):
        print(f"{i + 1}: {port}")

    # Ask the user to select a port
    while True:
        try:
            choice = int(input("Select the port number to connect: ")) - 1
            if 0 <= choice < len(available_ports):
                port = available_ports[choice]
                break
            else:
                print("Invalid selection. Please try again.")
        except ValueError:
            print("Please enter a valid number.")

    # Serial communication settings
    baudrate = 9216000
    timeout = 5

    try:
        # Open the selected serial connection
        with serial.Serial(port, baudrate, timeout=timeout) as ser:
            print(f"Connected to {port} at {baudrate} baud.")

            # Define the three command sequences
            commands_1 = [
                "255SP500",
                "255S200",
                "255I5000",
                "255A1",
            ]
            commands_2 = [
                "255SP2",
                "255S1",
                "255I5000",
                "255A1",
            ]
            commands_3 = [
                "255SW2000",
                "255S50",
                "255I5000",
                "255A1",
            ]
            
            # Combine them in a list to cycle through
            all_commands = [commands_1, commands_2, commands_3]
            while True:
                # Repeat the command cycles 90 times
                for i in range(32):
                    print(f"Iteration {i + 1} of 10:")
                    # Cycle through the command sets
                    for cmd_set in all_commands:
                        for cmd in cmd_set:
                            ser.write(f"{cmd}\n".encode("utf-8"))
                            print(f"Sent: {cmd}")
                            time.sleep(0.001)  # 1 milliseconds
                    

                    
                # Send the start command for the current iteration
                print("Commands sent in this iteration.")
                ser.write(f"255ST1\n".encode("utf-8"))

                input("Press Enter to stop...")  # Wait for Enter press to repeat
                ser.write(f"255A0\n".encode("utf-8"))
                ser.write(f"255SC1\n".encode("utf-8"))

                input("Press Enter to repeat...")  # Wait for Enter press to repeat

    except serial.SerialException as e:
        print(f"Serial error: {e}")
    except Exception as e:
        print(f"Error: {e}")


if __name__ == "__main__":
    main()
