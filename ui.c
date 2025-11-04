#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ncurses.h>


int startGame(int firstTime) {
    char* userInput = NULL;
    size_t size = 0;
	int gameOver = 0;
	int win = 0;
	do {
		if(firstTime == 1 ) {
			system("clear");
			printf("Welcome to a small little town called Lesfort. There is a small population of\n45 people. In this town the people hold up a tradition called the Lottey, which will soon take place.\n Maybe you should leave the house to go the townsquare to talk with the villagers.");
			printf("\n Before starting to get to know the villagers let's learn the controls: ");
			
		}
		ssize_t len = getline(&userInput, &size, stdin); //grabs user input
		userInput[len - 1] = '\0'; 
	} while(gameOver != 1);
    return win;
}

int ui() {
    char* userInput = NULL;
    size_t size = 0;
    int firstTime = 1;
    do {
        system("clear");
        printf("--------------------------------------------------------------------------------------------\n");
        printf("Welcome to The Lottery \n");
        printf("By: Bianca Rivera Nales \n");
        printf("\nTo start the game press 's' to quit press 'q': ");
        ssize_t len = getline(&userInput, &size, stdin); //grabs user input
        userInput[len - 1] = '\0'; //shaves off the newline at the end
   
        if(strcmp(userInput,"s") == 0) {
            startGame(firstTime);
        } else if(strcmp(userInput,"q") != 0){
            printf("Unknown option choose again.\n");
        }
    } while( strcmp(userInput,"q") != 0);
    free(userInput);
    return 0;
}