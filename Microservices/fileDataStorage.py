# Code written by Beck Johnson and Nora Jacobi with small changes implemented by Sophia Peroutka

import re
import zmq

context = zmq.Context()
PORT = 5555
SEPARATOR = b'\x21' # ! ASCII

# First part: Matches any amount of alphanumeric characters, parenthesis,
#   brackets, dots, or spaces
# Second part: Must match previous character set AND not end with
#   a period or space
PATTERN = re.compile(r"^[\w\-()\[\]\.\ ]*[\w\-()\[\]]$")

def write(filename, mode, data):
    with open(filename, mode) as file:
        if file.tell() != 0:
            file.write("\n")
        file.write(data)
    return

# NEW -- Extract a field from buffer
def extract(buffer, sep):
    idx = buffer.find(sep)
    if idx == -1:
        return None
    field = buffer[0:idx].decode()
    del buffer[:idx + 1]
    return field

# NEW -- Execute the requested file operation
def process_operation(operation, filename, data):
    match operation:
        case "append":
            write(filename, "a+", data)
        case "overwrite":
            write(filename, "w+", data)
        case "delete":
            write(filename, "w+", "")
        case _:
            return False, b'Error: Invalid operation. Must be "append", "overwrite", or "delete".'

    return True, b"Success: Data saved"

# NEW -- Gets the file name fromt he start of the buffer
def get_filename(buffer):
        # Get filename
        filename_idx = buffer.find(SEPARATOR)
        filename = buffer[0:filename_idx].decode()
        del buffer[:filename_idx + 1]
        return filename

# NEW -- Validates the file name
def validate_filename(filename, PATTERN, socket):
        # Make sure filename is valid
        match = PATTERN.fullmatch(filename)
        if not match:
            socket.send(b"Error: Invalid file name.")
            print()
            return False
        return True

if __name__ == "__main__":
    socket = context.socket(zmq.REP)
    socket.bind(f"tcp://*:{PORT}")

    print(f"Separator: {SEPARATOR.decode()}")


    while True:
        #  Wait for next request from client
        message = socket.recv()
        print(f"Recieved message: {message}")
        buffer = bytearray()
        buffer.extend(message)

        # NEW -- Get desired operation
        operation = extract(buffer, SEPARATOR)
        if operation is None:
            socket.send(b"Error: Malformed message.")
            print()
            continue

        print(f"Mode: {operation}")

        # NEW -- Get the file name
        filename = get_filename(buffer)
        print(f"Filename: {filename}")

        #NEW -- Validate the file name
        if not validate_filename(filename, PATTERN, socket):
            continue

        # Get data to write
        data = buffer.decode()
        print(f"Data: {data}")

        # NEW -- Get proccess the operation and get a response
        ok, response = process_operation(operation, filename, data)

        #  Send reply back to client
        socket.send(response)
        print()