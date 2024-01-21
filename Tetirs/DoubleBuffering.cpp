#include "DoubleBuffering.h"

// HANDLE 인덱스에 접근해서 버퍼를 교체시키는 변수
int screenIndex = 0;

// 버퍼의 크기
const int width = 40 * 2;
const int height = 24;

// 버퍼 생성 (2개)
HANDLE Screen[2];	// [0] front buffer
// [1] back buffer

void ScreenInit()
{
	CONSOLE_CURSOR_INFO cursor;

	// 버퍼의 가로 사이즈, 세로 사이즈
	COORD size = { width, height };

	// Left, Top, Right, Bottom
	SMALL_RECT rect = { 0, 0, width - 1, height - 1 };

	// 화면 2개 생성

	// front buffer
	Screen[0] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		nullptr,
		CONSOLE_TEXTMODE_BUFFER,
		nullptr);
	SetConsoleScreenBufferSize(Screen[0], size);
	SetConsoleWindowInfo(Screen[0], true, &rect);

	// front buffer
	Screen[1] = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		nullptr,
		CONSOLE_TEXTMODE_BUFFER,
		nullptr);
	SetConsoleScreenBufferSize(Screen[1], size);
	SetConsoleWindowInfo(Screen[1], true, &rect);

	// 커서의 활성화 여부
	// false : 거짓
	// true  : 참
	cursor.dwSize = 1;
	cursor.bVisible = false;

	SetConsoleCursorInfo(Screen[0], &cursor);
	SetConsoleCursorInfo(Screen[1], &cursor);
}

void ScreenFlipping()
{
	// 버퍼는 하나만 활성화 가능
	SetConsoleActiveScreenBuffer(Screen[screenIndex]);
	screenIndex = !screenIndex;
}

void ScreenClear()
{
	COORD coord{ 0, 0 };
	DWORD dw;

	FillConsoleOutputCharacter(Screen[screenIndex],
		L' ',
		width * height,
		coord,
		&dw);
}

void ScreenRelease()
{
	CloseHandle(Screen[0]);
	CloseHandle(Screen[1]);
}

void ScreenPrint(int x, int y, const char* string)
{
	COORD cursorPosition = { x * 2, y };
	DWORD dw;

	SetConsoleCursorPosition(Screen[screenIndex], cursorPosition);
	WriteFile(Screen[screenIndex],
		string,
		strlen(string),
		&dw,
		nullptr);
}


void TextColor(int color)
{
	SetConsoleTextAttribute(Screen[screenIndex], color);
}