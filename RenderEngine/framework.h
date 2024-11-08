#pragma once

#define WIN32_LEAN_AND_MEAN

#include <windows.h>

#ifdef RENDERENGINE_EXPORTS
#define RENDERENGINE_API __declspec(dllexport)
#else
#define RENDERENGINE_API __declspec(dllimport)
#endif