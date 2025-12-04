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
    baudrate = 921600
    timeout = 5

    try:
        # Open the selected serial connection
        with serial.Serial(port, baudrate, timeout=timeout) as ser:
            print(f"Connected to {port} at {baudrate} baud.")

            while True:  # Continuous loop
                input("Press Enter to send commands...")  # Wait for Enter press
                commands = [
                    "1SW1000",
                    "1T10",
                    "1I2000",
                    "1A1",
                    # ****** # TESTING SEQUENCE WITH TIME DURATION IN MS
                    "1SW1000",
                    "1S100",
                    "1I2000",
                    "1A1",
                    ######
                    "1SW1000",
                    "1S10",
                    "1I1000",
                    "1A1",
                    #######
                    "1SW1000",
                    "1S50",
                    "1I4000",
                    "1A1",
                    #######
                    "1SW1000",
                    "1H100",
                    "1I1000",
                    "1A1",
                    # ****** # TESTING SEQUENCE WITH TIME DURATION IN MS
                    # ****** # TESTING SEQUENCE WITH DURATION IN HALF PERIODS
                    "1SH10",
                    "1H5",
                    "1I-4000",
                    "1A1",
                    #######
                    "1SH3",
                    "1S1",
                    "1I4000",
                    "1A1",
                    #######
                    "1SH10",
                    "1T5",
                    "1I2000",
                    "1A1",
                    # ****** # TESTING SEQUENCE WITH DURATION IN HALF PERIODS
                    # ****** # TESTING SEQUENCE WITH DURATION IN FULL PERIODS
                    "1SP200",
                    "1H100",
                    "1I2000",
                    "1A1",
                    #######
                    "1SP100",
                    "1S50",
                    "1I4000",
                    "1A1",
                    #######
                    "1SP20",
                    "1T10",
                    "1I3000",
                    "1A1",
                    # ****** # TESTING SEQUENCE WITH DURATION IN FULL PERIODS
                    # ****** # START
                    "1ST1",
                ]

                for cmd in commands:
                    ser.write(f"{cmd}\n".encode("utf-8"))
                    print(f"Sent: {cmd}")

                print("Commands sent.")
                input("Press Enter to continue...")  # Wait for Enter press to repeat

    except serial.SerialException as e:
        print(f"Serial error: {e}")
    except Exception as e:
        print(f"Error: {e}")


if __name__ == "__main__":
    main()
