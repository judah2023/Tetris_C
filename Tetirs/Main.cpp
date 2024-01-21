#include "Main.h"

#include "DoubleBuffering.h"

#pragma comment(lib,"winmm.lib")
#include <mmsystem.h>

char tetrominos[7][4][4][5] = {
	// J Block
	{
		{
			"0000",
			"4000",
			"4440",
			"0000"
		},{
			"0000",
			"0440",
			"0400",
			"0400"
		},{
			"0000",
			"0000",
			"4440",
			"0040",
		},{
			"0000",
			"0400",
			"0400",
			"4400"
		},
	},

	// L Block
	{
		{
			"0000",
			"0040",
			"4440",
			"0000"
		},{
			"0000",
			"0400",
			"0400",
			"0440"
		},{
			"0000",
			"0000",
			"4440",
			"4000"
		},{
			"0000",
			"4400",
			"0400",
			"0400"
		},
	},

	// S Block
	{
		{
			"0000",
			"0440",
			"4400",
			"0000"
		},{
			"0000",
			"0400",
			"0440",
			"0040"
		},{
			"0000",
			"0000",
			"0440",
			"4400"
		},{
			"0000",
			"4000",
			"4400",
			"0400"
		},
	},

	// Z Block
	{
		{
			"0000",
			"4400",
			"0440",
			"0000"
		},{
			"0000",
			"0040",
			"0440",
			"0400"
		},{
			"0000",
			"0000",
			"4400",
			"0440"
		},{
			"0000",
			"0400",
			"4400",
			"4000"
		},
	},

	// T Block
	{
		{
			"0000",
			"0400",
			"4440",
			"0000"
		},{
			"0000",
			"0400",
			"0440",
			"0400"
		},{
			"0000",
			"0000",
			"4440",
			"0400"
		},{
			"0000",
			"0400",
			"4400",
			"0400"
		},
	},

	// I Block
	{
		{
			"0000",
			"4444",
			"0000",
			"0000"
		},{
			"0040",
			"0040",
			"0040",
			"0040"
		},{
			"0000",
			"0000",
			"4444",
			"0000"
		},{
			"0400",
			"0400",
			"0400",
			"0400"
		},
	},

	// O Block
	{
		{
			"0000",
			"0440",
			"0440",
			"0000"
		},{
			"0000",
			"0440",
			"0440",
			"0000"
		},{
			"0000",
			"0440",
			"0440",
			"0000"
		},{
			"0000",
			"0440",
			"0440",
			"0000"
		},
	},
};
char gameMap[HEIGTH][WIDTH + 1];
int  gameMapColor[HEIGTH][WIDTH];

Item item[SHOP_MAX] = {
		{ICON_SYSTEM, ExitShop}, 
		{ICON_ITEM, LineClear},
		{ICON_ITEM, LineClear5},
		{ICON_ITEM, LineClear10},
		{ICON_ITEM, LevelDown},
		{ICON_ITEM, LevelUp},
		{0},
		{0},
		{0},
		{0}};
Icon shop[SHOP_MAX];
GroupItem Inventory[INVENTORY_MAX];

int prices[SHOP_MAX] = { 0,10,40,80,30,5,0,0,0,0 };

char numberString[21], stringBuffer[50];

Pos iSRSOffset[8][5] = {
	{{0,0},{-2,0},{+1,0},{-2,-1},{+1,+2}},
	{{0,0},{-1,0},{+2,0},{-1,+2},{+2,-1}},
	{{0,0},{+2,0},{-1,0},{+2,+1},{-1,-2}},
	{{0,0},{+1,0},{-2,0},{+1,-2},{-2,+1}},
};
Pos otherSRSOffset[8][5] = {
	{{0,0},{-1,0},{-1,+1},{0,-2},{-1,-2}},
	{{0,0},{+1,0},{+1,-1},{0,+2},{+1,+2}},
	{{0,0},{+1,0},{+1,+1},{0,-2},{+1,-2}},
	{{0,0},{-1,0},{-1,-1},{0,+2},{-1,+2}},
};

Pos holdMapPos = { 1, 1 }, gameMapPos = { 6, 1 }, nextMapPos = { 17, 1 },
scorePos = { 22, 1 }, blockStartPos = { 4, 0 }, levelPos = { 1, 6 },
shopStartPos = { 6, 2 }, shopSpacePos = { 6, 5 }, inventoryPos = { 25, 7 },
scriptPos = { 3, 16 };

clock_t prevClock, currClock, dropDelay = 1;
time_t startTime;

unsigned bestScore, score, lines, 
level, levelUpExp, gold;
bool isGameOver, isShop;

std::queue<Player> nextTetris;
Tetris holdTetris;

int main()
{
	srand((unsigned)time(NULL));

	Player player;


	while (true)
	{
		ShopInit(player);

		while (isShop)
		{
			ShopPrint(player);

			ChooseItem(player);

			// 버퍼 교체
			ScreenFlipping();

			// 교체된 버퍼 비우기
			ScreenClear();
		}

		// 게임 초기화
		GameInit(player);
		clock_t dropTimer = 0;

		while (!isGameOver)
		{
			dropTimer += DeltaTime();
			GamePrint(player);
			MoveTetris(player, dropTimer);

			// 버퍼 교체
			ScreenFlipping();

			// 교체된 버퍼 비우기
			ScreenClear();
		}

		while (isGameOver)
			Keyboard(player);

		// 종료 시, 버퍼를 해제
		ScreenRelease();
	}

	// 종료 시, 버퍼를 해제
	ScreenRelease();
	return 0;
	
}

clock_t DeltaTime()
{
	clock_t deltaTime = currClock - prevClock;
	prevClock = currClock;
	currClock = clock();

	return deltaTime;
}

int CreateBType()
{
	return rand() % 7;
}

Player CreateNewTetris()
{
	Tetris newTetris = { CreateBType(), 0 };
	Player newPlayer = { blockStartPos, newTetris };
	return newPlayer;
}

COLORS GetTetrisColor(Player player)
{
	int bType = player.status.bType;
	switch (bType)
	{
	case 0:
		return DARK_BLUE;
	case 1:
		return DARK_YELLOW;
	case 2:
		return GREEN;
	case 3:
		return DARK_RED;
	case 4:
		return DARK_PURPLE;
	case 5:
		return DARK_SKY_BLUE;
	case 6:
		return YELLOW;
	default:
		return WHITE;
		break;
	}
}

COLORS GetShopColor(int itemNum)
{
	switch (itemNum)
	{
	case 0:
		return DARK_BLUE;
	case 1:
		return DARK_YELLOW;
	case 2:
		return GREEN;
	case 3:
		return DARK_RED;
	case 4:
		return DARK_PURPLE;
	case 5:
		return DARK_SKY_BLUE;
	default:
		return GRAY;
		break;
	}
}


void LoadBestScore(const char* dataFileName)
{
	FILE* file;
	fopen_s(&file, dataFileName, "r");
	if (file == nullptr)
	{
		SaveBestScore(dataFileName);
		fopen_s(&file, dataFileName, "r");
		if (file == nullptr)
			return;
	}

	fscanf_s(file, "%d", &bestScore);

	fclose(file);
}

void SaveBestScore(const char* dataFileName)
{
	FILE* file;
	fopen_s(&file, dataFileName, "w");
	if (file == nullptr)
		return;

	fprintf_s(file, "%d", bestScore);

	fclose(file);

}

void LoadInventory(const char* dataFileName)
{
	FILE* file;
	fopen_s(&file, dataFileName, "r");
	if (file == nullptr)
	{
		gold = 30;
		for (int i = 0; i < INVENTORY_MAX; i++)
		{
			if (item[i].mapStatus == ICON_ITEM)
			{
				Inventory[i].item = item[i];
				Inventory[i].stock = 0;
			}
		}
		SaveInventory(dataFileName);
		fopen_s(&file, dataFileName, "r");
	}
	
	fscanf_s(file, "%d", &gold);
	for (int i = 0; i < INVENTORY_MAX; i++)
	{
		if (item[i].mapStatus == ICON_ITEM)
		{
			Inventory[i].item = item[i];
			fscanf_s(file, "%d", &Inventory[i].stock);
		}
	}
	fclose(file);
}

void SaveInventory(const char* dataFileName)
{
	FILE* file;
	fopen_s(&file, dataFileName, "w");
	fprintf_s(file, "%d", gold);
	for (int i = 1; i < INVENTORY_MAX; i++)
		if (item[i].mapStatus == ICON_ITEM)
		{
			fprintf_s(file, "%c", '\n');
			fprintf_s(file, "%d", Inventory[i].stock);
		}
	fclose(file);
}


void ReadMapData(const char* MapFileName)
{
	FILE* file;
	fopen_s(&file, MapFileName, "r");
	for (int i = 0; i < HEIGTH; i++)
		fgets(gameMap[i], WIDTH + 2, file);

	fclose(file);
}


void ItemInit()
{
	FILE* file;
	fopen_s(&file, "MapData\\ItemData.txt", "r");
	for (int i = 0; i < SHOP_MAX; i++)
	{
		int x = i % 5;
		int y = i / 5;
		shop[i].item = item[i];
		shop[i].pos.x = shopStartPos.x + shopSpacePos.x * x;
		shop[i].pos.y = shopStartPos.y + shopSpacePos.y * y;
			shop[i].Price = prices[i];
			fgets(shop[i].descrption, SCRIPT_SIZE, file);
	}
	fclose(file);

	LoadInventory("MapData\\ItemStock.txt");
}

void ShopInit(Player& player)
{
	system("mode con cols=80 lines=24");
	isShop = 1;
	ScreenInit();
	ItemInit();

	ReadMapData("MapData\\Shop.txt");

	int color;
	for (int i = 0; i < SHOP_MAX; i++)
	{
		gameMap[shop[i].pos.y][shop[i].pos.x] = shop[i].item.mapStatus;
		color = GetShopColor(i);
		gameMapColor[shop[i].pos.y][shop[i].pos.x] = color;
	}

	char cell;
	for (int i = 0; i < HEIGTH; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			cell = gameMap[i][j];
			switch (cell)
			{
			case EMPTY:
				gameMapColor[i][j] = WHITE;
				break;
			case WALL:
				gameMapColor[i][j] = DARK_GREEN;
				break;
			case ICON_SYSTEM:
				gameMapColor[i][j] = WHITE;
			default:
				break;
			}
		}
	}

	player.status.bType = 0;
	PlaySound(TEXT("Sound\\Shop.wav"), nullptr, SND_FILENAME | SND_ASYNC | SND_LOOP | SND_NODEFAULT);	// 반복 재생
}

void ShopPrint(Player player)
{
	UpdataGameMap();

	TextColor(WHITE);
	for (int i = 0; i < SHOP_MAX; i++)
	{
		if (shop[i].item.mapStatus == ICON_ITEM)
		{
			sprintf_s(numberString, "%2d", shop[i].Price);
			ScreenPrint(shop[i].pos.x + BEZEL, shop[i].pos.y + BEZEL + 1, numberString);
		}
	}

	ScreenPrint(player.pos.x + BEZEL, player.pos.y + BEZEL, "△");

	sprintf_s(numberString, "%04d", gold);
	strcpy_s(stringBuffer, "소지금액 : ");
	strcat_s(stringBuffer, numberString);
	ScreenPrint(27 + BEZEL, scriptPos.y + BEZEL, stringBuffer);
}

void ChooseItem(Player& player)
{
	Keyboard(player);
	player.pos = shop[player.status.bType].pos;
	player.pos.y += 2;
	PrintScript(player);
}

void PrintScript(Player player)
{
	ScreenPrint(3 + BEZEL, 14 + BEZEL, shop[player.status.bType].descrption);
	if (shop[player.status.bType].item.mapStatus == ICON_ITEM)
	{
		sprintf_s(numberString, "%d", Inventory[player.status.bType].stock);
		strcpy_s(stringBuffer, "소지 수 : ");
		strcat_s(stringBuffer, numberString);
		ScreenPrint(scriptPos.x + BEZEL, scriptPos.y + BEZEL, stringBuffer);
	}
}

void SelectItem(Player player)
{
	if (gold >= shop[player.status.bType].Price && shop[player.status.bType].item.mapStatus == ICON_ITEM)
	{
		Inventory[player.status.bType].stock++;
		gold -= shop[player.status.bType].Price;
	}
	else if (shop[player.status.bType].item.mapStatus == ICON_SYSTEM)
		shop[player.status.bType].item.function();
}


void LineClear()
{
	for (int i = 0; i < GAME_WIDTH; i++)
	{
		gameMap[gameMapPos.y + GAME_HEIGHT - BEZEL][gameMapPos.x + i] = EMPTY;
		gameMapColor[gameMapPos.y + GAME_HEIGHT - BEZEL][gameMapPos.x + i] = BLACK;
	}

	PullDownBlocks(GAME_HEIGHT);
}

void LineClear5()
{
	for (int i = 0; i < 5; i++)
		LineClear();
}

void LineClear10()
{
	for (int i = 0; i < 10; i++)
		LineClear();
}

void LevelUp()
{
	level++;
}

void LevelDown()
{
	if (level > 0)
		level--;
}

void ExitShop()
{
	isShop = 0;
}


void GameInit(Player& player)
{
	ScreenInit();

	bestScore = score = lines = levelUpExp = isGameOver = 0;
	level = 1;
	LoadBestScore("BestScore\\data.txt");

	ReadMapData("MapData\\Game.txt");

	char cell;
	for (int i = 0; i < HEIGTH; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			cell = gameMap[i][j];
			switch (cell)
			{
			case EMPTY:
				gameMapColor[i][j] = BLACK;
				break;
			case WALL:
				gameMapColor[i][j] = DARK_GREEN;
				break;
			default:
				break;
			}
		}
	}

	int color;
	for (int i = 1; i < INVENTORY_MAX; i++)
	{
		if (Inventory[i].item.mapStatus == ICON_ITEM)
		{
			color = GetShopColor(i);
			gameMap[inventoryPos.y][inventoryPos.x + (i - 1) * 2] = Inventory[i].item.mapStatus;
			gameMapColor[inventoryPos.y][inventoryPos.x + (i - 1) * 2] = color;
		}
	}

	while (!nextTetris.empty())
		nextTetris.pop();

	for (int n = 0; n < NEXT_MAX; n++)
	{
		Player newTetris = CreateNewTetris();
		nextTetris.push(newTetris);
		color = GetTetrisColor(newTetris);

		for (int i = 0; i < TETRIS_DRAW; i++)
		{
			for (int j = 0; j < TETRIS_DRAW; j++)
			{
				gameMap[nextMapPos.y + 4 * n + i][nextMapPos.x + j] = tetrominos[newTetris.status.bType][newTetris.status.bRotate][i][j];
				gameMapColor[nextMapPos.y + 4 * n + i][nextMapPos.x + j] = color;
			}
		}
	}

	player = CreateNewTetris();
	holdTetris = { -1, 0 };

	startTime = time(NULL);
	currClock = clock();
	PlaySound(TEXT("Sound\\tetrismusic.wav"), nullptr, SND_FILENAME | SND_ASYNC | SND_LOOP);	// 반복 재생
}

void GamePrint(Player player)
{

	UpdataGameMap();
	UpdataPlayer(player);
	UpdateText();
	UpdataInventory();
}

void UpdataGameMap()
{
	char cell;
	int color;
	for (int i = 0; i < HEIGTH; i++)
	{
		for (int j = 0; j < WIDTH; j++)
		{
			cell = gameMap[i][j];
			color = gameMapColor[i][j];

			TextColor(color);
			switch (cell)
			{
			case EMPTY:
				ScreenPrint(j + BEZEL, i + BEZEL, " ");
				break;
			case BLOCK_STOP:
			case BLOCK_MOVE:
				ScreenPrint(j + BEZEL, i + BEZEL, "▣");
				break;
			case FLOOR:
			case WALL:
				ScreenPrint(j + BEZEL, i + BEZEL, "■");
				break;
			case ICON_ITEM:
				ScreenPrint(j + BEZEL, i + BEZEL, "◆");
				break;
			case ICON_SYSTEM:
				ScreenPrint(j + BEZEL, i + BEZEL, "♬");
				break;
			default:
				break;
			}
		}
	}
}

void UpdataPlayer(Player player)
{
	int color = GetTetrisColor(player);
	TextColor(color);
	for (int i = 0; i < TETRIS_DRAW; i++)
	{
		for (int j = 0; j < TETRIS_DRAW; j++)
		{
			if (tetrominos[player.status.bType][player.status.bRotate][i][j] == BLOCK_MOVE)
			{
				ScreenPrint(gameMapPos.x + player.pos.x + j, gameMapPos.y + player.pos.y + i, "▣");
			}
		}
	}
}

void UpdateText()
{
	TextColor(WHITE);

	sprintf_s(numberString, "%02d", level);
	strcpy_s(stringBuffer, "Level ");
	strcat_s(stringBuffer, numberString);
	ScreenPrint(levelPos.x + BEZEL, levelPos.y + BEZEL, stringBuffer);

	sprintf_s(numberString, "%04d", bestScore);
	strcpy_s(stringBuffer, " Best Score : ");
	strcat_s(stringBuffer, numberString);
	ScreenPrint(scorePos.x + BEZEL, scorePos.y + BEZEL, stringBuffer);

	sprintf_s(numberString, "%04d", score);
	strcpy_s(stringBuffer, " Score\t  : ");
	strcat_s(stringBuffer, numberString);
	ScreenPrint(scorePos.x + BEZEL, scorePos.y + BEZEL + 1, stringBuffer);

	sprintf_s(numberString, "%04d", lines);
	strcpy_s(stringBuffer, " Line\t  : ");
	strcat_s(stringBuffer, numberString);
	ScreenPrint(scorePos.x + BEZEL, scorePos.y + BEZEL + 2, stringBuffer);

	time_t currTime = time(NULL);
	time_t timer = currTime - startTime;

	strcpy_s(stringBuffer, " Time : ");

	sprintf_s(numberString, " %02llu", timer / 3600);
	strcat_s(stringBuffer, numberString);
	strcat_s(stringBuffer, " : ");

	timer %= 3600;
	sprintf_s(numberString, "%02llu", timer / 60);
	strcat_s(stringBuffer, numberString);
	strcat_s(stringBuffer, " : ");

	timer %= 60;
	sprintf_s(numberString, "%02llu", timer);
	strcat_s(stringBuffer, numberString);

	ScreenPrint(scorePos.x + BEZEL, scorePos.y + BEZEL + 4, stringBuffer);
}

void UpdataInventory()
{
	Pos printPos = inventoryPos;
	for (int i = 0; i < INVENTORY_MAX; i++)
	{
		if (Inventory[i].item.mapStatus == ICON_ITEM)
		{
			sprintf_s(numberString, "%02d", Inventory[i].stock);
			ScreenPrint(printPos.x + BEZEL, printPos.y + BEZEL + 1, numberString);
			printPos.x += 2;
		}
	}
}

void UpdateNextTetris()
{
	for (int n = 1; n < NEXT_MAX; n++)
		for (int i = 0; i < TETRIS_DRAW; i++)
			for (int j = 0; j < TETRIS_DRAW; j++)
			{
				gameMap[nextMapPos.y + 4 * (n - 1) + i][nextMapPos.x + j] = gameMap[nextMapPos.y + 4 * n + i][nextMapPos.x + j];
				gameMapColor[nextMapPos.y + 4 * (n - 1) + i][nextMapPos.x + j] = gameMapColor[nextMapPos.y + 4 * n + i][nextMapPos.x + j];

			}

	nextTetris.pop();
	Player newTetris = CreateNewTetris();
	nextTetris.push(newTetris);
	int color = GetTetrisColor(newTetris);
	for (int i = 0; i < TETRIS_DRAW; i++)
		for (int j = 0; j < TETRIS_DRAW; j++)
		{
			gameMap[nextMapPos.y + 4 * (NEXT_MAX - 1) + i][nextMapPos.x + j] = tetrominos[newTetris.status.bType][newTetris.status.bRotate][i][j];
			gameMapColor[nextMapPos.y + 4 * (NEXT_MAX - 1) + i][nextMapPos.x + j] = color;
		}
}

void UpdateTetris(Player& player)
{
	player = nextTetris.front();
	UpdateNextTetris();
}


void Keyboard(Player& player)
{
	Pos& playerPos = player.pos;
	char key = 0;
	if (_kbhit())
	{
		key = _getch();
		if (key == -32)
			key = _getch();

		if (isShop)
		{
			switch (key)
			{
			case UP:
				player.status.bType -= 5;
				break;
			case LEFT:
				player.status.bType--;
				break;
			case RIGHT:
				player.status.bType++;
				break;
			case DOWN:
				player.status.bType += 5;
				break;
			case KEY_Z:
			case KEY_z:
				SelectItem(player);
				break;
			default:
				break;
			}

			if (player.status.bType > 9 || player.status.bType < 0)
			{
				player.status.bType += 10;
				player.status.bType %= 10;
			}
		}
		else if (!isGameOver)
		{
			switch (key)
			{
			case UP:
				RotationTetris(player.status.bRotate);
				break;
			case LEFT:
				playerPos.x--;
				break;
			case RIGHT:
				playerPos.x++;
				break;
			case DOWN:
				playerPos.y++;
				break;
			case KEY_Z:
			case KEY_z:
				HoldTetris(player);
				break;
			case KEY_1:
			case KEY_2:
			case KEY_3:
			case KEY_4:
			case KEY_5:
			case KEY_6:
			case KEY_7:
			case KEY_8:
			case KEY_9:
				if (Inventory[key - '0'].stock > 0)
				{
					Inventory[key - '0'].item.function();
					Inventory[key - '0'].stock--;
				}
				break;
			default:
				break;
			}
		}
		else
		{
			switch (key)
			{
			case KEY_R:
			case KEY_r:
				isGameOver = 0;
				break;
			default:
				break;
			}
		}

	}
}

void MoveTetris(Player& player, clock_t& dropTimer)
{
	Player prevStatus = player;
	Keyboard(player);

	CollideCheck(player, prevStatus);

	if (dropTimer * level >= dropDelay * 1000)
	{
		DropTetris(player);
		dropTimer = 0;
	}
}

void DropTetris(Player& player)
{
	Player prevStatus = player;
	Pos prevPos = player.pos;

	player.pos.y++;

	CollideCheck(player, prevStatus);
}


void RotationTetris(int& rotate)
{
	rotate = (rotate + 1) % 4;
}

void SRSSystem(Player& player, int prevRotate)
{
	if (player.status.bType < 5) // I, O 블럭은 회전점의 위치가 다르므로 Offset이 달라진다.
	{
		Player testPlayer;
		for (int i = 0; i < 5; i++)
		{
			testPlayer = player;
			testPlayer.pos.x += otherSRSOffset[player.status.bRotate][i].x;
			testPlayer.pos.y += otherSRSOffset[player.status.bRotate][i].y;
			if (!isCollide(testPlayer))
			{
				player.pos.x = testPlayer.pos.x;
				player.pos.y = testPlayer.pos.y;
				return;
			}
		}
	}
	else 
	{
		Player testPlayer;
		for (int i = 0; i < 5; i++)
		{
			testPlayer = player;
			testPlayer.pos.x += iSRSOffset[player.status.bRotate][i].x;
			testPlayer.pos.y += iSRSOffset[player.status.bRotate][i].y;
			if (!isCollide(testPlayer))
			{
				player.pos.x = testPlayer.pos.x;
				player.pos.y = testPlayer.pos.y;
				return;
			}

		}
	}

	player.status.bRotate = prevRotate;
}

void HoldTetris(Player& player)
{
	int color = GetTetrisColor(player);
	if (holdTetris.bType == -1)
	{
		holdTetris = player.status;
		UpdateTetris(player);
	}
	else
	{
		Tetris tempStatus = holdTetris;
		holdTetris = player.status;
		player = { blockStartPos,tempStatus };
	}
	holdTetris.bRotate = 0;

	for (int i = 0; i < TETRIS_DRAW; i++)
	{
		for (int j = 0; j < TETRIS_DRAW; j++)
		{
			if (tetrominos[holdTetris.bType][holdTetris.bRotate][i][j] == BLOCK_MOVE)
			{
				gameMap[holdMapPos.x + i][holdMapPos.y + j] = BLOCK_STOP;
				gameMapColor[holdMapPos.x + i][holdMapPos.y + j] = color;
			}
			else
			{
				gameMap[holdMapPos.x + i][holdMapPos.y + j] = EMPTY;
				gameMapColor[holdMapPos.x + i][holdMapPos.y + j] = BLACK;
			}
		}
	}
}


bool isCollide(Player player)
{
	Pos collidePos = { gameMapPos.x + player.pos.x - BEZEL ,
					   gameMapPos.y + player.pos.y - BEZEL };
	for (int i = 0; i < TETRIS_DRAW; i++)
	{
		for (int j = 0; j < TETRIS_DRAW; j++)
		{
			if (tetrominos[player.status.bType][player.status.bRotate][i][j] == 
				BLOCK_MOVE && gameMap[collidePos.y + i][collidePos.x + j] > EMPTY)
			{
				return true;
			}
		}
	}
	return false;
}

void StopTetris(Player& player)
{
	int color = GetTetrisColor(player);
	for (int i = 0; i < TETRIS_DRAW; i++)
		for (int j = 0; j < TETRIS_DRAW; j++)
			if (tetrominos[player.status.bType][player.status.bRotate][i][j] == BLOCK_MOVE)
			{
				gameMap[gameMapPos.y + player.pos.y + i - BEZEL][gameMapPos.x + player.pos.x + j - BEZEL] = BLOCK_STOP;
				gameMapColor[gameMapPos.y + player.pos.y + i - BEZEL][gameMapPos.x + player.pos.x + j - BEZEL] = color;

			}


	GameLogic(player);
	UpdateTetris(player);
}

void CollideCheck(Player& player, Player prevStatus)
{
	Pos prevPos = prevStatus.pos;
	int prevRotate = prevStatus.status.bRotate;

	if (player.status.bRotate != prevRotate)
	{
		SRSSystem(player, prevRotate);
	}
	else
	{
		if (isCollide(player))
		{
			if (player.pos.y > prevPos.y)
			{
				player.pos = prevPos;
				StopTetris(player);
			}
			else
			{
				player.pos = prevPos;
			}
		}
	}
}


void GameOverLogic(Player player)
{
	if (player.pos.y == blockStartPos.y)
	{
		isGameOver = 1;
		SaveBestScore("BestScore\\data.txt");
		SaveInventory("MapData\\ItemStock.txt");
		TextColor(RED);
		ScreenPrint(gameMapPos.x + 5, gameMapPos.y + 10, "Game Over...");
		TextColor(RED);
		ScreenPrint(gameMapPos.x + 2, gameMapPos.y + 11, "Press R Key To Restart");
		gold += score / 10;
		PlaySound(TEXT("Sound\\gameover.wav"), nullptr, SND_FILENAME | SND_ASYNC| SND_NODEFAULT);	// 반복 재생
	}
}

bool IsClearLine(int lineNum)
{
	for (int j = 0; j < GAME_WIDTH; j++)
		if (gameMap[gameMapPos.y + lineNum - BEZEL][gameMapPos.x + j] != BLOCK_STOP)
			return false;
	return true;
}

bool IsAboveBlock(int lineNum)
{
	for (int j = 0; j < GAME_WIDTH; j++)
		if (gameMap[gameMapPos.y + lineNum - BEZEL][gameMapPos.x + j] != EMPTY)
			return true;
	return false;
}

void PullDownBlocks(int startRow)
{
	for (int k = 1; k < GAME_HEIGHT; k++)
	{
		startRow--;
		if (IsAboveBlock(startRow))
		{
			for (int j = 0; j < GAME_WIDTH; j++)
				if (gameMap[gameMapPos.y + startRow - BEZEL][gameMapPos.x + j] < FLOOR)
				{
					gameMap[gameMapPos.y + startRow - BEZEL + 1][gameMapPos.x + j] =
						gameMap[gameMapPos.y + startRow - BEZEL][gameMapPos.x + j];
					gameMapColor[gameMapPos.y + startRow - BEZEL + 1][gameMapPos.x + j] =
						gameMapColor[gameMapPos.y + startRow - BEZEL][gameMapPos.x + j];

				}
		}
		else
		{
			for (int j = 0; j < GAME_WIDTH; j++)
			{
				gameMap[gameMapPos.y + startRow - BEZEL + 1][gameMapPos.x + j] = EMPTY;
				gameMapColor[gameMapPos.y + startRow - BEZEL + 1][gameMapPos.x + j] = BLACK;
			}
			break;
		}
	}
}

void UpdateScore(int lineCnt)
{
	score += lineCnt * lineCnt;
	lines += lineCnt;
	if (bestScore < score)
		bestScore = score;

	levelUpExp += lineCnt;
	if (levelUpExp >= 10)
	{
		level++;
		levelUpExp -= 10;
	}
}

void LineClearLogic(Player player)
{
	int lineCnt = 0;
	for (int i = 0; i < TETRIS_DRAW; i++)
	{
		int startRow = player.pos.y + i;
		if (IsClearLine(startRow))
		{
			++lineCnt;
			PullDownBlocks(startRow);
		}
	}

	UpdateScore(lineCnt);
}

void GameLogic(Player player)
{
	GameOverLogic(player);
	LineClearLogic(player);
}