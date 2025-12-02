#include <zmq.h>
#include <jansson.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>


// SAVES USER DATA
int main () {
    printf ("Connecting to save user server…\n");
    void *context = zmq_ctx_new();
    void *socket = zmq_socket(context, ZMQ_REQ);
    zmq_connect(socket, "tcp://localhost:5458");

    json_t *req = json_object();
    json_object_set_new(req, "index", json_integer(1));
    json_object_set_new(req, "username", json_string("test"));
    json_object_set_new(req, "items", json_string(1 == 1 ? "glasses" : "no_glasses"));
	json_object_set_new(req, "favor", json_integer(0));
	json_t *questsJSON = json_object();
	for(int i = 0; i < 1; i++) {
		char* status = "pending";
        int completed = 1;
        int accepted = 0;
		if(completed == 1) {
			status = "completed";
		} else if(accepted == 1) {
			status = "in_progress";
		}
		json_object_set_new(questsJSON, "Missing Glasses", json_string(status));
	}
	json_object_set_new(req, "quests", questsJSON);

    char *jsonData = json_dumps(req, JSON_COMPACT);

    zmq_send(socket, jsonData, strlen(jsonData), 0);
    free(jsonData);
    json_decref(req);
    char* reply = malloc(2048);
    if (!reply) return 0;
    int received = zmq_recv(socket, reply, 2047, 0);
    if (received < 0) {
        free(reply);
        return 0;
    }
    reply[received] = '\0';  // ensure null-termination
    printf("Returned data: %s\n", reply);
    zmq_close (socket);
    zmq_ctx_destroy (context);
    return 0;
}

// LOADS IN USER DATA
// int main () {
//     printf ("Connecting to load user server…\n");
//     void *context = zmq_ctx_new();
//     void *socket = zmq_socket(context, ZMQ_REQ);
//     zmq_connect(socket, "tcp://localhost:5459");

//     json_t *req = json_object();
//     json_object_set_new(req, "index", json_integer(1));
//     json_object_set_new(req, "username", json_string("test"));
//     char *jsonData = json_dumps(req, JSON_COMPACT);

//     zmq_send(socket, jsonData, strlen(jsonData), 0);
//     free(jsonData);
//     json_decref(req);
//     char* reply = malloc(2048);
//     if (!reply) return 0;
//     int received = zmq_recv(socket, reply, 2047, 0);
//     if (received < 0) {
//         free(reply);
//         return 0;
//     }
//     reply[received] = '\0';  // ensure null-termination
//     printf("Returned data: %s\n", reply);
//     zmq_close (socket);
//     zmq_ctx_destroy (context);
//     return 0;
// }


// LOGS IN PLAYER!!!
// int main () {
//     printf ("Connecting to load user server…\n");
//     void *context = zmq_ctx_new();
//     void *socket = zmq_socket(context, ZMQ_REQ);
//     zmq_connect(socket, "tcp://localhost:5556");

//     json_t *req = json_object();
//     json_object_set_new(req, "csv_file", json_string("login.csv"));
//     json_object_set_new(req, "username", json_string("test"));
//     json_object_set_new(req, "password", json_string("password"));
//     char *jsonData = json_dumps(req, JSON_COMPACT);

//     zmq_send(socket, jsonData, strlen(jsonData), 0);
//     free(jsonData);
//     json_decref(req);
//     char reply[2048];
// 	int received = zmq_recv(socket, reply, sizeof(reply) - 1, 0);
//     reply[received] = '\0';
//     printf("Returned data: %s\n", reply);
//     zmq_close (socket);
//     zmq_ctx_destroy (context);
// }


// GETS USER INDEX!!!
// int userIndex = 0;

// int main() {
//     void *context = zmq_ctx_new();
// 	void *socket = zmq_socket(context, ZMQ_REQ);
// 	zmq_connect(socket, "tcp://localhost:5557");
// 	char* data = "get!login.csv";
// 	// I want to get back the new index needed to append the new user's data to
	
// 	printf("sending data...%s\n", data);

// 	zmq_send(socket, data, strlen(data), 0);
// 	char reply[2048];
// 	int received = zmq_recv(socket, reply, sizeof(reply) - 1, 0);
// 	reply[received] = '\0';
// 	userIndex = atoi(reply);

// 	printf("User index %d\n", userIndex);

// 	zmq_close (socket);
// 	zmq_ctx_destroy (context);
// }