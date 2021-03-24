#pragma once

#include <pch.h>
#include <GameApplicationLayer/GameApp.h>

// TODO: move it to Game View

void ResizeWindow(WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WindowProcedure(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);