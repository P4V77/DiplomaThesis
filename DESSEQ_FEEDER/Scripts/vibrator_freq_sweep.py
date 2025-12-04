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
    baudrate = 921600  # Adjust baud rate as required
    timeout = 5  # Timeout for reading messages

    try:
        # Open the selected serial connection
        with serial.Serial(port, baudrate, timeout=timeout) as ser:
            if not ser.is_open:
                ser.open()
            print(f"Connected to {port} at {baudrate} baud.")

            # Prepare commands for start at 1 Hz
            commands = [
                "255H1",
                "255RT15",
                "255R1000",
                "1V7000",
                "2V7000",
                "1P1",
                "2P1",
                # "3P1",
                # "4P1",
            ]
            for i, cmd in enumerate(commands):
                ser.write(f"{cmd}\n".encode("utf-8"))  # Send the command
                print(f"Sent: {cmd}")
                if i == 1:  # Wait 5 seconds after sending 255P1
                    print(f"Vibrating at {1} Hz for 1 sec ...")
                    time.sleep(1)
         
            for x in range(2, 200, 5):
                # Prepare commands
                command_255Hx = f"255H{x}"  # Dynamically include the value of `x`
                commands = [command_255Hx]
                # Send the commands
                for i, cmd in enumerate(commands):
                    ser.write(f"{cmd}\n".encode("utf-8"))  # Send the command
                    print(f"Sent: {cmd}")
                    print(f"Vibrating at {x} Hz for 1 sec ...")
                    time.sleep(1)

            # Turn off
            ser.write(f"255P0\n".encode("utf-8"))  # Send the command
            print("Turning off")

    except serial.SerialException as e:
        print(f"Serial error: {e}")
    except Exception as e:
        print(f"Error: {e}")


if __name__ == "__main__":
    main()
