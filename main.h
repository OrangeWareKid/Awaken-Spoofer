#pragma once

#include "includes.h"

HWND hwnd;
RECT rc;
ImVec2 MenuSize = { 700, 455 };
HRGN hRgn = CreateRoundRectRgn(0, 0, MenuSize.x, MenuSize.y, 20, 20);