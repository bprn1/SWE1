import csv
import json
import zmq

def saveUser(updatedRow, filename):
    rows = []
    try:
        #Open the provided file for reading
        with open(filename, newline="", encoding="utf8") as file:
            reader = csv.DictReader(file)
            rows = list(reader)
    #If the file cant be found return 0
    except FileNotFoundError:
        return 0
    #For each row update items and quests
    found = False
    if not rows:
        return 0
    try: 
        for row in rows:
            if int(row["index"]) == int(updatedRow["index"]):
                try: 
                    row["quests"] = json.loads(row["quests"])
                except:
                    row["quests"] = {}
                row["items"] = updatedRow["items"]
                try:
                    row["quests"] = json.dumps(updatedRow["quests"])
                except Exception as e:
                    print("Error json encoding quests: ", e)
                row["favor"] = updatedRow["favor"]
                found = True
                break
    except Exception as e:
        print("Error json encoding: ", e)
        return 0
    if not found:
        return 0
    #Now try to write back to the csv
    try:
        with open(filename, "w", newline="", encoding="utf8") as file:
            writer = csv.DictWriter(file, fieldnames=rows[0].keys())
            writer.writeheader()
            writer.writerows(rows)
        return 1
    #If the file cant be found return 0
    except FileNotFoundError:
        return 0

#Make sure the message recieved from user is correct
def validate_info(msg):
    index = msg.get("index")
    username = msg.get("username")
    items = msg.get("items")
    favor = msg.get("favor")
    quests = msg.get("quests")

    if index is None or username is None or items is None or favor is None or quests is None:
        return None, None, None, None, None, False
    
    return index, username, items, favor, quests, True

def main():
    #Create zmq context
    context = zmq.Context()
    #Create a socket at port 5458
    socket = context.socket(zmq.REP)
    socket.bind("tcp://*:5458")

    print("Save user microservice running on port 5458...", flush=True)

    while True:
        #Recieve the message
        message = socket.recv_json()

        index, username, items, favor, quests, valid = validate_info(message)

        #If request is not valid give respond with an error
        if not valid:
            socket.send_json("ERROR user input is wrong")
            continue

        updatedRow = {
            "index": index,
            "username": username,
            "items": items,
            "quests": quests,
            "favor": favor
        }
        print(f"Row: {updatedRow}")

        bool = saveUser(updatedRow, "userData.csv")
        if bool:
            socket.send_json("Saved successfully")
        else:
            socket.send_json("ERROR could not save")
        continue

if __name__ == "__main__":
    main()
