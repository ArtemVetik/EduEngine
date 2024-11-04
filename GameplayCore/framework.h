#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#ifdef GAMEPLAYCORE_EXPORTS
#define GAMEPLAYCORE_API __declspec(dllexport)
#else
#define GAMEPLAYCORE_API __declspec(dllimport)
#endif