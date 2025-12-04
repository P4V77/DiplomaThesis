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
            print(f"Connected to {port} at {baudrate} baud.")

            while True:
                # Ask the user to specify the value of `x` for the command `255Hx`
                while True:
                    try:
                        x = int(input("Negative numbers exit script \nEnter frequency: "))
                        break

                    except ValueError:
                        print("Invalid input. Please enter a valid unsigned number.")

                # Prepare commands
                command_255Hx = f"255H{x}"  # Dynamically include the value of `x`
                commands = [command_255Hx, "255P1", "255P0"]
                if x < 0:
                    break

                # Send the commands
                for i, cmd in enumerate(commands):
                    ser.write(f"{cmd}\n".encode("utf-8"))  # Send the command
                    print(f"Sent: {cmd}")
                    if i == 1:  # Wait until the user presses Enter after sending 255P1
                        print("Vibrating... Press Enter to stop.")
                        input()  # Wait for user to press Enter

    except serial.SerialException as e:
        print(f"Serial error: {e}")
    except Exception as e:
        print(f"Error: {e}")


if __name__ == "__main__":
    main()
