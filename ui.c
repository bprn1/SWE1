#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>

typedef enum {
	STATE_HOME,
	STATE_TOWN_SQUARE,
	STATE_EXIT
} GameState;

typedef struct {
	GameState location;
	char name[50];
	int favor;
} Player;

void HomeOptions(Player *player) {
	clear();
	nodelay(stdscr, FALSE);
	printw("Home options here.\n");
	printw("Insert 1 to go to the town\n");
	printw("Insert 2 to inspect house\n");
	refresh();
	echo();
	int c = getch();
	if(c == '1' ) {
		player->location = STATE_TOWN_SQUARE;
	} else if( c == '2' ) {
		printw("\nYou find nothing except your bed and an alarm clock on your bed side table\n");
		refresh();
	} else {
		printw("\nInvalid input try again..\n");
		refresh();
	}
	noecho();
	nodelay(stdscr, TRUE);
}

void TownOptions(Player *player) {
	clear();
	nodelay(stdscr, FALSE);
	printw("Town options here.\n");
	printw("Insert 1 to go home\n");
	printw("Insert 2 to talk to Edward\n");
	refresh();
	echo();
	int c = getch();
	if(c == '1' ) {
		player->location = STATE_HOME;
	} else if( c == '2' ) {
		printw("Hello %s, what can I do for you?\n", player->name);
		refresh();
	} else {
		printw("Invalid input try again..\n");
		refresh();
	}
	noecho();
	nodelay(stdscr, TRUE);
}

void Quests() {
	printw("Quests shown here.\n");
	refresh();
}

void PauseMenu() {
	printw("Commands shown here.\n");
	refresh();
}

void Commands() {
	printw("Commands shown here.\n");
	refresh();
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
		noecho();
		nodelay(stdscr, TRUE);
		clear();
		printw("Welcome %s to a small little town called Lesfort. There is a small population of 15 people. In this town the people hold up a tradition called the Lottery, which will soon take place. The goal in the game is to gain the most favor to avoid winning the Lottery. Maybe you should leave the house to go the townsquare to talk with the villagers.", player->name);
		printw("Press Enter to continue\n");
		refresh();
		c = getch();
		clear();

		printw("\nBefore starting to get to know the villagers let's learn the controls: \n");
		printw("Press q for Quests \nPress esc for Pause Menu \nPress t for a list of Commands which you can configure\n");
		printw("Press Enter to continue\n");
		refresh();
		c = getch();
		firstTime = 0; 
		clear();
	}

	while(gameOver != 1){
		c = getch();
		if(c != ERR) {
			if(c == 'q')
				Quests();
			if(c == 27)
				PauseMenu();
			if(c == 't')
				Commands();
			if(c == 'f'){
				printw("Favor: %d", player->favor);
				refresh();
			}
		}

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
		}
	}
	if(player->favor > 5) {
		return win = 1;
	} else {
		return win = 0;
	}
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
			Player player = {STATE_HOME, "", 0};
			int win = startGame(&player, firstTime);
			refresh();
			if(win == 0) {
				printw("You lost the game :(, maybe be nicer to the villagers next time...");
				refresh();
			} else if(win == 2) {
				printw("You quit the game, we hope to have a save function next time you play!");
				refresh();
			} else {
				printw("You won the game! Congrats!");
				refresh();
			}
		} else if(c != 'q') {
			printw("Unknown option choose again.\n");
			refresh();
		}
	}
	endwin();
    return 0;
}