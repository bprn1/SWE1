// #include <ncurses.h>

// #define BOARD 20
// #define BOARD_ROWS BOARD
// #define BOARD_COLS BOARD * 2.5

// int xMax = 0;
// int yMax = 0;

// char *choices[] = { 
// 			"Choice 1",
// 			"Choice 2",
// 			"Choice 3",
// 			"Choice 4",
// 			"Exit",
// 		  };
// int n_choices = sizeof(choices) / sizeof(char *);
// void print_menu(WINDOW *board_win, int highlight);

// int main()
// {	WINDOW *board_win; // a pointer the the ncurses window
// 	int highlight = 1; //keeps track of which menu item is currently selected
// 	int choice = 0; // stores the user's final choice
// 	int c; // holds keyboard input from user

// 	initscr(); //initializes the screen
// 	clear();
// 	noecho(); //doesn't print everything typed out
// 	cbreak(); //no need for enter	/* Line buffering disabled. pass on everything */
// 	getmaxyx(stdscr, yMax, xMax);
		
// 	board_win = newwin(BOARD_ROWS, BOARD_COLS, (yMax/2) - (BOARD_ROWS/2), (xMax/2) - (BOARD_COLS/2)); //returns a new window that represents that area
// 	keypad(board_win, TRUE); //special keys have names not random chars
//     box(board_win, 0, 0);
// 	mvprintw(0, 0, "Welcome to a small little town in Maine, called Lesfort. Here there is a small population of\n45 people. In this town the people hold up a tradition called the Lottey, which will soon take place. You should get up before the ceremony begins to talk with the townsfolk.");
//     mvprintw(1, 0, "\n Before starting to get to know the villagers let's learn the controls: "); // prints to screen a y x
// 	refresh(); // shows changes
// 	print_menu(board_win, highlight);
// 	while(1)
// 	{	c = wgetch(board_win);
// 		switch(c)
// 		{	case KEY_UP:
//             case 'w':
// 				if(highlight == 1)
// 					highlight = n_choices;
// 				else
// 					--highlight;
// 				break;
// 			case KEY_DOWN:
//             case 's':
// 				if(highlight == n_choices)
// 					highlight = 1;
// 				else 
// 					++highlight;
// 				break;
// 			case 10:
// 				choice = highlight;
// 				break;
// 			default:
// 				mvprintw(24, 0, "Charcter pressed is = %3d Hopefully it can be printed as '%c'", c, c);
// 				refresh();
// 				break;
// 		}
// 		print_menu(board_win, highlight);
// 		if(choice != 0)	/* User did a choice come out of the infinite loop */
// 			break;
// 	}	
// 	mvprintw(23, 0, "You chose choice %d with choice string %s\n", choice, choices[choice - 1]);
// 	clrtoeol();
// 	refresh();
// 	endwin();
// 	return 0;
// }


// void print_menu(WINDOW *board_win, int highlight)
// {
// 	int x, y, i;	

// 	x = 2;
// 	y = 2;
// 	box(board_win, 0, 0);
// 	for(i = 0; i < n_choices; ++i)
// 	{	if(highlight == i + 1) /* High light the present choice */
// 		{	wattron(board_win, A_REVERSE); 
// 			mvwprintw(board_win, y, x, "%s", choices[i]);
// 			wattroff(board_win, A_REVERSE);
// 		}
// 		else
// 			mvwprintw(board_win, y, x, "%s", choices[i]);
// 		++y;
// 	}
// 	wrefresh(board_win);
// }