#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>

typedef enum {
	STATE_HOME,
	STATE_TOWN_SQUARE,
	STATE_EXIT, 
	STATE_LOTTERY,
	STATE_PAUSE
} GameState;

typedef enum {
	EFFECT_NONE,
	EFFECT_FAVOR, 
	EFFECT_QUEST
} EffectType;

typedef struct {
	char name[50];
	char description[256]; // number of quests with description
	char hint[128]; //hints per quest
	int giveHint; // bool/flag for if user wants to show hint or not
	int accepted;
	int completed;
} QuestStruct;

typedef struct {
	GameState location;
	char name[50];
	int favor;
	QuestStruct quests[3]; //has 3 quests max 
	int questCount; //number of quests accepted
	GameState prevLocation;
	int glasses; //did the player find the glasses
} Player;

typedef struct {
	char text[256]; //what villager can say
	char options[3][128]; //3 options with text buffer
	int next[3];	// what dialog to go to after each option
	EffectType effect[3];
	int effectVal[3];
	int questIndex[3];
} Dialogue;

typedef struct {
	char name[50];
	Dialogue *dialogue;
	int num_dialogues;
} Villager;

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
	printw("3) Settings\n");
	printw("4) Achievements\n");
	printw("5) Back to game\n");
	refresh();
	int c = getch();
	switch(c) {
		case '1':
		case '2':
		case '3':
		case '4':
			printw("\nNot implemented will be soon!\n");
			printw("Press any button to continue\n");
			refresh();
			c = getch();
			break;
		case '5':
			player->location = player->prevLocation;
			player->prevLocation = STATE_PAUSE;
	}
	nodelay(stdscr, TRUE);
}

void Commands() {
	nodelay(stdscr, FALSE);
	clear();
	printw("========= Commands =========\n");
	printw("Press q for Quests \nPress esc for Pause Menu \nPress t for a list of Commands\n");
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
			
			//if(strlen(v->dialogue[current].options[i]) > 0 ) {
				printw("%d) %s", i + 1, v->dialogue[current].options[i]);
				refresh();
			//}
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
			printw("Press q for Quests \nPress esc for Pause Menu \nPress t for a list of Commands which you can configure\n");
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

int main() {
    int firstTime = 1;
	int c;
	initscr();
	//noecho();
	cbreak();
	keypad(stdscr, TRUE);
	printw("Welcome to The Lottery\n");
	printw("By: Bianca Rivera Nales\n");
	printw("To start the game press 's' to quit press 'q': ");
    refresh();
	while((c = getch()) != 'q') {
		if(c == 's') {
			Player player = {STATE_HOME, "", 0, {}, 0, STATE_PAUSE, 0};
			int win = startGame(&player, firstTime);
			clear();
			printw("Win value = %d\n", win);
			refresh();
			nodelay(stdscr, FALSE);
			if(win == 0) {
				printw("You got stoned to death and lost the game :( maybe be nicer to the villagers next time...\n");
				refresh();
			} else if(win == 2) {
				printw("You quit the game, we hope to have a save function next time you play!\n");
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