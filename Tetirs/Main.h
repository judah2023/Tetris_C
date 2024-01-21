#pragma once

#include <time.h>
#include <queue>

enum COLORS
{
	BLACK,
	DARK_BLUE,
	DARK_GREEN,
	DARK_SKY_BLUE,
	DARK_RED,
	DARK_PURPLE,
	DARK_YELLOW,
	GRAY,
	DARK_GRAY,
	BLUE,
	GREEN,
	SKY_BLUE,
	RED,
	PURPLE,
	YELLOW,
	WHITE,
};

enum ARROW_KEYS
{
	UP = 72,
	LEFT = 75,
	RIGHT = 77,
	DOWN = 80,
	SPACE = 32,

	KEY_R = 'R',
	KEY_r = 'r',
	KEY_Z = 'Z',
	KEY_z = 'z',
	
	KEY_0 = '0',
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
};

enum SIZE_CONSTANTS
{
	BEZEL = 1,
	WIDTH = 38,
	HEIGTH = 22,
	GAME_WIDTH = 10,
	GAME_HEIGHT = 20,
	TETRIS_DRAW = 4,
	NEXT_MAX = 5,
	SCORE_WIDTH = 15,
	SCORE_HEIGHT = 5,
	SHOP_MAX = 10,
	INVENTORY_MAX = 7,
	SCRIPT_SIZE = 50
};

enum MAP_STATUS
{
	EMPTY = '0',
	BLOCK_STOP,
	FLOOR,
	WALL,
	BLOCK_MOVE,
	ICON_ITEM,
	ICON_SYSTEM,
};

struct Pos
{
	int x;
	int y;
};

struct Tetris
{
	int bType;
	int bRotate;
};

struct Player
{
	Pos pos;
	Tetris status;
};

typedef  void (*ItemFunction)();

struct Item
{
	int mapStatus;
	ItemFunction function;
};

struct Icon
{
	Pos pos;
	Item item;
	int Price;
	char descrption[SCRIPT_SIZE];
};

struct GroupItem
{
	Item item;
	int stock;
};

clock_t DeltaTime();
int CreateBType();
Player CreateNewTetris();
COLORS GetTetrisColor(Player player);
COLORS GetShopColor(int itemNum);

void LoadBestScore(const char * dataFileName);
void SaveBestScore(const char* dataFileName);

void LoadInventory(const char* dataFileName);
void SaveInventory(const char* dataFileName);

void ReadMapData(const char* MapFileName);

void ItemInit();

void ShopInit(Player& player);
void ShopPrint(Player player);
void ChooseItem(Player& player);
void PrintScript(Player player);
void SelectItem(Player player);

void LineClear();
void LineClear5();
void LineClear10();
void LevelUp();
void LevelDown();
void ExitShop();

void GameInit(Player& player);
void GamePrint(Player player);
void UpdataGameMap();
void UpdataPlayer(Player player);
void UpdateText();
void UpdataInventory();
void UpdateNextTetris();
void UpdateTetris(Player& player);

void Keyboard(Player& player);
void MoveTetris(Player &player, clock_t &dropTimer);
void DropTetris(Player& player);

void RotationTetris(int &rotate);
void SRSSystem(Player& player, int prevrotate);

void HoldTetris(Player& player);

bool isCollide(Player player);
void StopTetris(Player &player);
void CollideCheck(Player& player, Player prevStatus);

void GameOverLogic(Player player);
bool IsClearLine(int lineNum);
bool IsAboveBlock(int lineNum);
void PullDownBlocks(int startRow);
void UpdateScore(int lineCnt);
void LineClearLogic(Player player);
void UpdateScore(int lineCnt);
void GameLogic(Player player);