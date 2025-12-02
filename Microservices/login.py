# Code mainly done by Sophie Peroutka
import csv
import zmq

#Check the given csv file for correct username and password
def check_login(filename, username, password):
    try:
        #Open the provided file for reading
        with open(filename, newline="") as file:
            reader = csv.reader(file)
            #For each row there are two things, a username and a password
            for row in reader:
                ind = row[0].strip()
                user = row[1].strip()
                pwd = row[2].strip()
                if user == username and pwd == password:
                    #If so return the index of the account
                    return int(ind)
    #If the file cant be found return 0
    except FileNotFoundError:
        return 0
    #If the username and pass combo doesnt exist return 0
    return 0

#Send response to the client
def send_response(socket, status, index):
    socket.send_json({
        "status": status,
        "index": index
    })

#Make sure the csv file username and password are valid and return them
def validate_request(msg):
    csv_file = msg.get("csv_file")
    username = msg.get("username")
    password = msg.get("password")

    #If the user did not supply a csv file, username, and password return valid as false
    if not csv_file or not username or not password:
        return None, None, None, False
    
    return csv_file, username, password, True

def main():
    #Create zmq context
    context = zmq.Context()
    #Create a socket at port 5556
    socket = context.socket(zmq.REP)
    socket.bind("tcp://*:5556")

    print("Sign-in microservice running on port 5556...", flush=True)

    while True:
        #Recieve the message
        msg = socket.recv_json()

        csv_file, username, password, valid = validate_request(msg)

        #If request is not valid give respond with an error
        if not valid:
            send_response(socket, "ERROR", 0)
            continue
        
        index = check_login(csv_file, username, password)

        #If the index is not zero than an account was found matching the user and pass, return the index
        if index != 0:
            send_response(socket, "OK", index)
        #If the index is zero return zero indicating there was no matching account
        else:
            send_response(socket, "FAIL", 0)


if __name__ == "__main__":
    main()