import serial
import time
import threading
from serial.tools import list_ports


def find_serial_ports():
    """Find all available serial ports."""
    ports = list_ports.comports()
    return [port.device for port in ports]


class ExitSignal:
    """A shared signal to indicate when the user wants to stop the program."""

    def __init__(self):
        self.exit = False

    def set_exit(self):
        self.exit = True

    def reset(self):
        self.exit = False

    def check(self):
        return self.exit


def monitor_user_input(exit_signal):
    """Thread function to monitor user input."""
    input("Press Enter to stop vibrating...")
    exit_signal.set_exit()


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
    frequency = 0

    try:
        # Open the selected serial connection
        with serial.Serial(port, baudrate, timeout=timeout) as ser:
            print(f"Connected to {port} at {baudrate} baud.")

            while True:
                # Resetting commands
                commands = [
                    "255P0",
                    "255R0000",
                    "255V4500",
                    "255R0000",
                    "255V4500",
                    "255F0",
                    "255F0",
                ]
                for cmd in commands:
                    ser.write(f"{cmd}\n".encode("utf-8"))  # Send the command

                # Get frequency input
                while True:
                    try:
                        frequency = int(input("Enter frequency: "))
                        if frequency < 0:
                            raise ValueError("Frequency must be a non-negative number.")
                        break
                    except ValueError:
                        print("Invalid input. Please enter a valid unsigned number.")

                # Ask the user to select a movement
                available_movements = [
                    "Left to Right",
                    "Right to Left",
                    "Center",
                    "Sweep",
                ]
                print("Available movements:")
                for i, movement in enumerate(available_movements):
                    print(f"{i + 1}: {movement}")

                while True:
                    try:
                        movement = int(input("Select movement number: ")) - 1
                        if 0 <= movement < len(available_movements):
                            selected_movement = available_movements[movement]
                            print(f"Selected movement: {selected_movement}")
                            break
                        else:
                            print("Invalid selection. Please try again.")
                    except ValueError:
                        print("Please enter a valid number.")

                # Initialize movement parameters
                sweep_l2r = True
                left_offset = "7000"
                left_amplitude = "100"
                left_phase = "0"
                right_offset = "7000"
                right_amplitude = "100"
                right_phase = "0"

                # Initialize exit signal for this session
                exit_signal = ExitSignal()
                input_thread = threading.Thread(
                    target=monitor_user_input, args=(exit_signal,)
                )
                input_thread.daemon = True

                # Start the vibration process
                input_thread.start()
                print("Vibrating... Press Enter to stop.")

                while not exit_signal.check():
                    match selected_movement:
                        case "Left to Right":
                            left_offset = "4500"
                            left_amplitude = "3500"
                            right_offset = "1000"
                            right_amplitude = "0"
                            right_phase = "270"
                            left_phase = "0"

                        case "Right to Left":
                            left_offset = "1500"
                            left_amplitude = "0500"
                            left_phase = "270"
                            right_offset = "7000"
                            right_amplitude = "1000"
                            right_phase = "0"

                        case "Center":
                            left_offset = "4500"
                            left_amplitude = "3000"
                            right_offset = "4500"
                            right_amplitude = "3000"
                            right_phase = "180"
                            left_phase = "0"

                        case "Sweep":
                            if sweep_l2r:
                                right_offset = str(int(right_offset) - 500)
                                if int(right_offset) < 2000:
                                    right_offset = "7000"
                                    sweep_l2r = False
                                right_amplitude = "1000"
                                right_phase = "270"
                            else:
                                left_offset = str(int(left_offset) - 500)
                                if int(left_offset) < 2000:
                                    left_offset = "7000"
                                    sweep_l2r = True
                                left_amplitude = "1000"
                                left_phase = "270"

                    commands = [
                        "255S" + f"{frequency}",
                        "1V" + left_offset,
                        "1R" + left_amplitude,
                        "2V" + left_offset,
                        "2R" + left_amplitude,
                        "5V" + right_offset,
                        "3R" + right_amplitude,
                        "3V" + right_offset,
                        "4R" + right_amplitude,
                        "4V" + right_offset,
                        "255P1",
                        # "3P0"
                        # "4P0"
                        # "1P1" + left_phase,
                        # "1P1" + left_phase,
                        "1F" + f"{0}",
                        "2F" + f"{0}",
                        "3F" + f"{0}",
                        "4F" + f"{0}",
                    ]

                    # Send the commands
                    for cmd in commands:
                        ser.write(f"{cmd}\n".encode("utf-8"))
                        print(f"Sent: {cmd}")

                    print("Press enter for stopping vibrators")
                    if selected_movement == "Sweep":
                        for _ in range(30):  # Sleep for 1.5s but check exit signal
                            if exit_signal.check():
                                break
                            time.sleep(0.05)
                    else:
                        while True:
                            if exit_signal.check():
                                break

                print("Vibration stopped.")
                exit_signal.reset()

    except serial.SerialException as e:
        print(f"Serial error: {e}")
    except Exception as e:
        print(f"Error: {e}")


if __name__ == "__main__":
    main()
