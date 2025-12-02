# Code written by Beck Johnson 
import csv
import json
import zmq

def loadUser(index, username, filename):
    try:
        with open(filename, newline="", encoding="utf8") as file:
            reader = csv.DictReader(file)
            for row in reader:
                if int(row["index"]) == index and row["username"] == username:
                    row["quests"] = json.loads(row["quests"])
                    row["favor"] = int(row["favor"])
                    #If so return the row of the user's data
                    return row
    #If the file can't be found return 0
    except FileNotFoundError:
        return 0
    #If the username and index doesn't exist return none
    return None

#Make sure the message recieved from user is correct
def validate_info(msg):
    index = msg.get("index")
    username = msg.get("username")

    if index is None or username is None:
        return None, None, False
    
    return index, username, True

def main():
    #Create zmq context
    context = zmq.Context()
    #Create a socket at port 5459
    socket = context.socket(zmq.REP)
    socket.bind("tcp://*:5459")

    print("Load user microservice running on port 5459...", flush=True)

    while True:
        #Recieve the message
        message = socket.recv_json()

        index, username, valid = validate_info(message)

        #If request is not valid give respond with an error
        if not valid:
            socket.send_json("ERROR user input is wrong")
            continue

        row = loadUser(index, username, "userData.csv")
        if row:
            socket.send_json(row)
        else:
            socket.send_json("ERROR could not load user data")
        continue

if __name__ == "__main__":
    main()
