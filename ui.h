#ifndef UI_H
#define UI_H

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

void Quests(Player *player);
void PauseMenu(Player *player);
void Commands();
void talkToVillager(Villager *v, Player *player);
void HomeOptions(Player *player);
void TownOptions(Player *player);
int startGame(Player *player, int firstTime);
void saveUserData(Player* player);
int loginUser();
void parsePlayerData(char* json_str, Player* player);
void loadUserInfo(Player* player);
void getIndex();
void newUser();


#endif