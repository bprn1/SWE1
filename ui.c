#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>
#include <zmq.h>
#include <jansson.h>
#include "ui.h"


// Global Variables for  player usage
int userIndex = 0;
char userName[50] = "\0";

QuestStruct questOptions[] = {
	{"Missing Glasses", "Find Edwards missing glasses that he dropped.", "Try inspecting the townsquare.", 0, 0, 0},
	{"Collect the Children", "Collect all 3 of the Smith's kids", "Maybe try bribbing them with candy.", 0, 0, 0},
	{"Barter with the Baker", "Try to convice the baker they should trade bread for a lump of coal", "Maybe a few drinks will loosen their prices...", 0, 0, 0}
};

Dialogue Edward_dialogue[] = {
	{ "Hey there, %s! It seems I lost my glasses... What can I do for you?\n",
      {"Do you need any help finding your glasses? (Gain 5 favor if found)\n", "Can you tell me more about the town.\n", "How can I win the lottery.\n"},
      {1, 2, 3}, {EFFECT_NONE, EFFECT_NONE, EFFECT_NONE}, {0, 0, 0}, {-1, -1, -1}},

    { "Yes it would be great if you can find my glasses, I must've dropped them somewhere..\n", {"I'll try to find them...\n", "Maybe later.\n"}, {-1, -1}, {EFFECT_QUEST, EFFECT_NONE}, {0, 0}, {0, -1}},
    { "The townsfolk mainly works in the fields, to provide for the town. There isn't much to do here.\n", {"Hmmm alright.\n", "Goodbye.\n"}, {-1, -1}, {EFFECT_NONE, EFFECT_NONE}, {0, 0}, {-1, -1}},
    { "You want to win the lottery?!? I think you mean you want to avoid winning it.\n", {"Why?\n", "No I think I want to win it. (Lose 10 favor)\n"}, {4, -1}, {EFFECT_NONE, EFFECT_FAVOR}, {0, -10}, {-1, -1}},

	{ "You'll die if you win the lottery, you want to avoid winning by gaining favor with us villagers.\n", {"Oh, I see.\n", "Thanks for the heads up! (Gain 1 favor)\n"}, {-1,-1}, {EFFECT_NONE, EFFECT_FAVOR}, {0, 1}, {-1, -1}}
};

Dialogue Anne_dialogue[] = {
	{ "Hey there, %s! I think Derrel is going to win the lottery today.\n",
      {"Why?\n", "Did you rig the lottery?? (Lose 5 favor)\n", "Yeah, he was close to winning last year... (Gain 2 favor for Gossip)\n"},
      {1, 2, 3}, {EFFECT_NONE, EFFECT_FAVOR, EFFECT_FAVOR}, {0, -5, 2}, {-1, -1}},

    { "He hasn't been doing well in the fields lately, so I think it's his time.\n", {"I thought he was doing well...\n", "Hmm... See you later\n"}, {-1, -1}, {EFFECT_NONE, EFFECT_NONE}, {0, 0}, {-1, -1}},
    { "How could you imply such a thing!\n", {"...\n", "Sorry.\n"}, {-1, -1}, {EFFECT_NONE, EFFECT_NONE}, {0, 0}, {-1, -1}},
    { "Yeah I bet he'll draw the paper this year.\n", {"I guess we'll see.\n", "He will win!\n"}, {-1, -1}, {EFFECT_NONE, EFFECT_NONE}, {0, 0}, {-1, -1}}
};

void Quests(Player *player) {
	nodelay(stdscr, FALSE);
	clear();
	printw("========= Quests =========\n");
	for( int i = 0; i < player->questCount; i++) {
		printw("Quest: %s\n", player->quests[i].name);
		printw("	%s\n", player->quests[i].description);
		if(player->quests[i].giveHint) {
			printw("Hint: %s\n", player->quests[i].hint);
		} else {
			printw("Press 'h' to see a hint for all quests\n Reopen quest book to see hints\n");
		}
	}
	printw("Press any button to continue\n");
	refresh();

	int c = getch();
	if( c == 'h') {
		for( int i = 0; i < player->questCount; i++) {
			player->quests[i].giveHint = 1;
		}
	}
	nodelay(stdscr, TRUE);
}

void PauseMenu(Player *player) {
	nodelay(stdscr, FALSE);
	clear();
	printw("========= Pause Menu =========\n");
	printw("1) Save\n");
	printw("2) Save and Exit\n");
	printw("3) Back to game\n");
	refresh();
	int c = getch();
	switch(c) {
		case '1':
			saveUserData(player);
			break;
		case '2':
			saveUserData(player);
			player->location = STATE_EXIT;
			player->prevLocation = STATE_PAUSE;
			break;
		case '3':
			player->location = player->prevLocation;
			player->prevLocation = STATE_PAUSE;
			break;
		default:
			printw("Incorrect key pressed, please try again!\n");
			refresh();
			napms(1000);
			break;
	}
	nodelay(stdscr, TRUE);
}

void Commands() {
	nodelay(stdscr, FALSE);
	clear();
	printw("========= Commands =========\n");
	printw("Press q for Quests \nPress esc for Pause Menu \nPress t for a list of Commands \nPress f to check your favor with the villagers\n");
	printw("Press Enter to continue\n");
	refresh();
	getch();
	nodelay(stdscr, TRUE);
}

void talkToVillager(Villager *v, Player *player) {
	int current = 0;
	int c;

	while( current >= 0) {
		clear();
		printw("%s says: ", v->name);
		printw(v->dialogue[current].text, player->name);
		printw("\n");
		for(int i = 0; i < 3; i++) {
			if(strlen(v->dialogue[current].options[i]) == 0) {
				continue;
			}
			
			if(v->dialogue[current].effect[i] == EFFECT_QUEST) {
				int qIndex = v->dialogue[current].questIndex[i];
				if(qIndex >= 0 && player->quests[qIndex].accepted) {
					continue;
				}
			}
			
			printw("%d) %s", i + 1, v->dialogue[current].options[i]);
			refresh();
		}

		refresh();
		c = getch();

		if(c >= '1' && c <= '3') {
			int choice = c - '1';
			EffectType effect = v->dialogue[current].effect[choice];
			int value = v->dialogue[current].effectVal[choice];

			if(effect == EFFECT_FAVOR) {
				player->favor += value;
			} else if (effect == EFFECT_QUEST) {
				int qIndex = v->dialogue[current].questIndex[choice];
				if(qIndex >= 0 && !player->quests[qIndex].accepted) {
					player->quests[qIndex] = questOptions[qIndex]; //put it into players quest book
					player->quests[qIndex].accepted = 1;
					player->questCount++;
					printw("\nQuest added: %s\n", player->quests[qIndex].name);
					printw("Press any key to continue");
					refresh();
					c = getch();
				}
			}
			current = v->dialogue[current].next[choice];
		} else if (c == 27) {
			PauseMenu(player);
		} else if(c == 'q') {
			Quests(player);
		} else if(c == 't') {
			Commands();
		} else if(c == 'f') {
			printw("\nFavor: %d\n", player->favor);
			refresh();
			napms(1000);
		} else {
			printw("\nInvalid choice. Press 1-3.\n");
			refresh();
			napms(1000);
		}
	}

	clear();
	printw("You finished talking to %s.\n", v->name);
	refresh();
	getch();
}

void HomeOptions(Player *player) {
	clear();
	nodelay(stdscr, FALSE);
	printw("========= Home =========\n");
	printw("1) To go to the town\n");
	printw("2) To inspect house\n");
	refresh();
	echo();
	int c = getch();
	if(c == '1' ) {
		player->prevLocation = player->location;
		player->location = STATE_TOWN_SQUARE;
	} else if( c == '2' ) {
		printw("\nYou find nothing except your bed and an alarm clock on your bed side table.\n");
		printw("Press enter to continue.\n");
		refresh();
		c= getch();
	} else if (c == 27) {
		player->prevLocation = player->location;
		player->location = STATE_PAUSE;
		PauseMenu(player);
	} else if(c == 'q') {
		Quests(player);
	} else if(c == 't') {
		Commands();
	} else if(c == 'f') {
		printw("\nFavor: %d\n", player->favor);
		refresh();
		napms(1000);
	} else {
		printw("\nInvalid input try again..\n");
		refresh();
		c = getch();
	}
	noecho();
	nodelay(stdscr, TRUE);
}

void TownOptions(Player *player) {
	clear();
	Villager Edward = {"Edward", Edward_dialogue, 4};
	Villager Anne = {"Anne", Anne_dialogue, 3};
	nodelay(stdscr, FALSE);
	printw("========= Town =========\n");
	printw("1) To go home\n");
	printw("2) To talk to Edward\n");
	printw("3) To talk to Anne\n");
	printw("4) To inspect town\n");
	printw("5) Start the lottery ceremony\n");
	refresh();
	echo();
	int c = getch();
	if(c == '1' ) {
		player->prevLocation = player->location;
		player->location = STATE_HOME;
	} else if( c == '2' ) {
		talkToVillager(&Edward, player);
	} else if (c == '3') {
		talkToVillager(&Anne, player);
	} else if (c == '4') {
		if(player->quests[0].accepted && !player->quests[0].completed) {
			printw("\nYou see something shinning in the Sun, mixed in with some trash on the floor.\n Do you pick up the item?\n");
			printw("Press y for yes or n to go back.\n");
			refresh();
			c = getch();
			if(c == 'y') {
				player->glasses = 1;
				printw("\nYou found Edward's glasses! You return them to him and gain 10 favor.\n Press any button to continue.\n");
				refresh();
				player->quests[0].completed = 1;
				player->favor += 10;
				getch();
			}
		} else {
			printw("\nThere seems to be a lot of trash on the ground, someone should really pick it up...\n");
			printw("Press any key to continue on your day.\n");
			refresh();
			c = getch();
		}
	} else if (c == '5') {
		printw("\nAre you sure you want to start the lottery? It will end the game.\n");
		printw("Press y for yes or n to go back.\n");
		refresh();
		c = getch();
		if(c == 'y') {
			player->prevLocation = player->location;
			player->location = STATE_LOTTERY;
		}
	} else if (c == 27) {
		player->prevLocation = player->location;
		player->location = STATE_PAUSE;
		PauseMenu(player);
	} else if(c == 'q') {
		Quests(player);
	} else if(c == 't') {
		Commands();
	} else if(c == 'f') {
		printw("\nFavor: %d\n", player->favor);
		refresh();
		napms(1000);
	} else {
		printw("\nInvalid input try again..\n");
		refresh();
		c = getch();
	}
	noecho();
	nodelay(stdscr, TRUE);
}

int startGame(Player *player, int firstTime) {
	int gameOver = 0;
	int win = 0;
	int c;
	nodelay(stdscr, TRUE);
	clear();
	if(firstTime == 1 ) {
		nodelay(stdscr, FALSE);
		printw("What's your name?: ");
		refresh();
		echo();
		getnstr(player->name, sizeof(player->name) -1);
		clear();
		printw("Welcome %s to a small little town called Lesfort. There is a small population of 15 people. In this town the people hold up a tradition called the Lottery, which will soon take place. The goal in the game is to gain the most favor to avoid winning the Lottery. Maybe you should leave the house to go the townsquare to talk with the villagers.", player->name);
		printw("\n\nPress 1 to skip, press enter to continue.\n");
		refresh();
		c = getch();
		clear();
		if(c != '1') {
			printw("\nBefore starting to get to know the villagers let's learn the controls: \n");
			printw("Press q for Quests \nPress esc for Pause Menu \nPress t for a list of Commands \nPress f to check your favor with the villagers\n");
			printw("Press Enter to continue\n");
			refresh();
			c = getch();
		}
		noecho();
		nodelay(stdscr, TRUE);
		firstTime = 0; 
		clear();
	}

	while(gameOver != 1){
		switch (player->location) {
			case STATE_HOME:
				HomeOptions(player);
				refresh();
				break;
			case STATE_TOWN_SQUARE:
				TownOptions(player);
				break;
			case STATE_EXIT:
				gameOver = 1;
				return win = 2;
				break;
			case STATE_LOTTERY:
				gameOver = 1;
				if(player->favor > 5) {
					return win = 1;
				} else {
					return win = 0;
				}
				break;
			case STATE_PAUSE:
				PauseMenu(player);
				break;
		}
	}
	return win;
}

// Microservices:

void saveUserData(Player* player) {
    void *context = zmq_ctx_new();
    void *socket = zmq_socket(context, ZMQ_REQ);
    zmq_connect(socket, "tcp://localhost:5458");
    json_t *req = json_object();
    json_object_set_new(req, "index", json_integer(userIndex));
    json_object_set_new(req, "username", json_string(userName));
	json_object_set_new(req, "items", json_string(player->glasses == 1 ? "glasses" : "no_glasses"));
	json_object_set_new(req, "favor", json_integer(player->favor));
	json_t *questsJSON = json_object();
	for(int i = 0; i <player->questCount; i++) {
		char* status = "pending";
		if(player->quests[i].completed == 1) {
			status = "completed";
		} else if(player->quests[i].accepted == 1) {
			status = "in_progress";
		}
		json_object_set_new(questsJSON, player->quests[i].name, json_string(status));
	}
	json_object_set_new(req, "quests", questsJSON);

    char *jsonData = json_dumps(req, JSON_COMPACT);

    zmq_send(socket, jsonData, strlen(jsonData), 0);
    free(jsonData);
    json_decref(req);
    char* reply = malloc(2048);
	if(!reply) return;
    int received = zmq_recv(socket, reply, 2047, 0);
	reply[received] = '\0';
    zmq_close (socket);
    zmq_ctx_destroy (context);
	printw("%s\n", reply);
	refresh();
	napms(1000);
}

int loginUser() {
	// login to an exsisting user file, use login microservice
	char username[50];
	char password[50];
	void *context = zmq_ctx_new();
	void *socket = zmq_socket(context, ZMQ_REQ);
	zmq_connect(socket, "tcp://localhost:5556");

	json_t *req = json_object();
	json_object_set_new(req, "csv_file", json_string("login.csv"));
	printw("\nEnter Username: ");
	getstr(username);
	printw("\nEnter Password: ");
	getstr(password);
	json_object_set_new(req, "username", json_string(username));
	json_object_set_new(req, "password", json_string(password));
	char *jsonData = json_dumps(req, JSON_COMPACT);
	zmq_send(socket, jsonData, strlen(jsonData), 0);
	free(jsonData);
	json_decref(req);
	char reply[2048];
	int received = zmq_recv(socket, reply, sizeof(reply) - 1, 0);
	reply[received] = '\0';

	json_error_t error;
	json_t *root = json_loads(reply, 0, &error);
	if (!root) {
		printf("JSON parse error: %s\n", error.text);
	} else {
		json_t *indexJSON = json_object_get(root, "index");
		json_t *statusJSON = json_object_get(root, "status");
		if(indexJSON && json_is_integer(indexJSON)) {
			userIndex = (int)json_integer_value(indexJSON);
		}
		if(statusJSON && json_is_string(statusJSON) && (strcmp(json_string_value(statusJSON), "FAIL") == 0 || strcmp(json_string_value(statusJSON), "ERROR") == 0)) {
			return 0;
		}
		json_decref(root);
	}
	strncpy(userName, username, sizeof(userName)-1);
	userName[sizeof(userName) -1] = '\0';
	zmq_close (socket);
	zmq_ctx_destroy (context);
	return 1;
}

void parsePlayerData(char* json_str, Player* player) {
	json_error_t error;
	json_t* root = json_loads(json_str, 0, &error);
	if(!root) {
		printf("JSON parse error: %s\n", error.text);
		return;
	}

	json_t* usernameJSON = json_object_get(root, "username");
	if(usernameJSON) {
		strncpy(player->name, json_string_value(usernameJSON), sizeof(player->name)-1);
	}
	json_t* itemsJSON = json_object_get(root, "items");
	if(itemsJSON) {
		const char* items = json_string_value(itemsJSON);
		player->glasses = (strcmp(items, "glasses") == 0) ? 1 : 0;
	}
	json_t* favorJSON = json_object_get(root, "favor");
    if (favorJSON) {
        player->favor = json_integer_value(favorJSON);
    }

    // Quests
    json_t* questsJSON = json_object_get(root, "quests");
    if (questsJSON && json_is_object(questsJSON)) {
        const char* key;
        json_t* value;
        size_t i = 0;
        json_object_foreach(questsJSON, key, value) {
            if (i >= 3) break;
            strncpy(player->quests[i].name, key, sizeof(player->quests[i].name)-1);
            const char* status = json_string_value(value);
            player->quests[i].completed = (strcmp(status, "completed") == 0) ? 1 : 0;
            player->quests[i].accepted = (strcmp(status, "in_progress") == 0) ? 1 : 0;
            i++;
        }
		player->questCount = i;
	}
	json_decref(root);
}

void loadUserInfo(Player* player) {
    void *context = zmq_ctx_new();
    void *socket = zmq_socket(context, ZMQ_REQ);
    zmq_connect(socket, "tcp://localhost:5459");
    json_t *req = json_object();
    json_object_set_new(req, "index", json_integer(userIndex));
    json_object_set_new(req, "username", json_string(userName));
    char *jsonData = json_dumps(req, JSON_COMPACT);

    zmq_send(socket, jsonData, strlen(jsonData), 0);
    free(jsonData);
    json_decref(req);
    char* reply = malloc(2048);
	if (!reply) return;
    int received = zmq_recv(socket, reply, 2047, 0);
	reply[received] = '\0';
    zmq_close (socket);
    zmq_ctx_destroy (context);
	parsePlayerData(reply, player);
}

void getIndex() {
	void *context = zmq_ctx_new();
	void *socket = zmq_socket(context, ZMQ_REQ);
	zmq_connect(socket, "tcp://localhost:5557");
	char* data = "get!login.csv";
	// I want to get back the new index needed to append the new user's data to
	
	printw("sending data...\n");
	refresh();

	zmq_send(socket, data, strlen(data), 0);
	char reply[2048];
	int received = zmq_recv(socket, reply, sizeof(reply) - 1, 0);
	reply[received] = '\0';
	userIndex = atoi(reply);

	printw("User index %d\n", userIndex);
	refresh();

	zmq_close (socket);
	zmq_ctx_destroy (context);
}

void newUser() {
	char username[50];
	char password[50];
	char data[256];
	printw("\nEnter Username: ");
	getstr(username);
	printw("\nEnter Password: ");
	getstr(password);
	getIndex();
	void *context = zmq_ctx_new();
	void *socket = zmq_socket(context, ZMQ_REQ);
	zmq_connect(socket, "tcp://localhost:5555");
	sprintf(data, "append!login.csv!%d,%s,%s", userIndex, username, password);
	
	printw("sending data... %s\n", data);
	refresh();

	zmq_send(socket, data, strlen(data), 0);
	char reply[2048];
	int received = zmq_recv(socket, reply, sizeof(reply) - 1, 0);
	reply[received] = '\0';
	strncpy(userName, username, sizeof(userName)-1);
	userName[sizeof(userName) -1] = '\0';

	printw("%s\n", reply);
	refresh();

	napms(1000);
	zmq_close (socket);
	zmq_ctx_destroy (context);
}

int main() {
    int firstTime = 1;
	int c;
	initscr();
	cbreak();
	keypad(stdscr, TRUE);
	printw("Welcome to The Lottery\n");
	printw("By: Bianca Rivera Nales\n");
	printw("To start the game and create a new save file press 's', to login to an exisitng save file press 'l', or to quit press 'q': ");
    refresh();
	while((c = getch()) != 'q') {
		if(c == 's' || c == 'l') {
			Player player = {STATE_HOME, "", 0, {}, 0, STATE_PAUSE, 0};
			if(c == 's') {
				// create a new login add to user's file at next available index
				newUser();
			}
			if(c == 'l') {
				int success = loginUser();
				if(success == 0) {
					printw("Incorrect login, please try again later.");
					refresh();
					napms(1000);
					break;
				}
				firstTime = 0;
				loadUserInfo(&player);
			}
			int win = startGame(&player, firstTime);
			clear();
			printw("Win value = %d\n", win);
			refresh();
			nodelay(stdscr, FALSE);
			if(win == 0) {
				printw("You got stoned to death and lost the game :( maybe be nicer to the villagers next time...\n");
				refresh();
			} else if(win == 2) {
				printw("You exited the game, come back and finish the game soon!\n");
				refresh();
			} else {
				printw("You avoided getting stoned (phew!), and won the game! Congrats!\n");
				refresh();
			}
			printw("Press any button to exit game.\n");
			c = getch();
			break;
		} else if(c != 'q') {
			printw("Unknown option choose again.\n");
			refresh();
		}
	}
	endwin();
    return 0;
}