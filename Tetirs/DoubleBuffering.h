#pragma once

#include <stdio.h>
#include <conio.h>
#include <Windows.h>

void ScreenInit();						// ���� �ʱ�ȭ �Լ�
void ScreenFlipping();					// ���۸� ��ü�ϴ� �Լ�
void ScreenClear();						// ��ü�� ���۸� �����ִ� �Լ�
void ScreenRelease();					// ���۸� �����ϴ� �Լ�
void ScreenPrint(int x, int y,			// ���� ���۸��� �̿��ؼ� ����ϴ� �Լ�
				 const char* string);
void TextColor(int color);