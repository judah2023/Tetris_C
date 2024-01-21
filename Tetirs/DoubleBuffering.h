#pragma once

#include <stdio.h>
#include <conio.h>
#include <Windows.h>

void ScreenInit();						// 버퍼 초기화 함수
void ScreenFlipping();					// 버퍼를 교체하는 함수
void ScreenClear();						// 교체된 버퍼를 지워주는 함수
void ScreenRelease();					// 버퍼를 해제하는 함수
void ScreenPrint(int x, int y,			// 더블 버퍼링을 이용해서 출력하는 함수
				 const char* string);
void TextColor(int color);