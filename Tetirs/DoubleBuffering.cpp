#include "DoubleBuffering.h"

// HANDLE �ε����� �����ؼ� ���۸� ��ü��Ű�� ����
int screenIndex = 0;

// ������ ũ��
const int width = 40 * 2;
const int height = 24;

// ���� ���� (2��)
HANDLE Screen[2];	// [0] front buffer
// [1] back buffer

void ScreenInit()
{
	CONSOLE_CURSOR_INFO cursor;

	// ������ ���� ������, ���� ������
	COORD size = { width, height };

	// Left, Top, Right, Bottom
	SMALL_RECT rect = { 0, 0, width - 1, height - 1 };

	// ȭ�� 2�� ����

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

	// Ŀ���� Ȱ��ȭ ����
	// false : ����
	// true  : ��
	cursor.dwSize = 1;
	cursor.bVisible = false;

	SetConsoleCursorInfo(Screen[0], &cursor);
	SetConsoleCursorInfo(Screen[1], &cursor);
}

void ScreenFlipping()
{
	// ���۴� �ϳ��� Ȱ��ȭ ����
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