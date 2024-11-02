#pragma once

#define WIN32_LEAN_AND_MEAN
#include <string>

#include "d3dx12.h"
#include "DxException.h"

#ifdef GRAPHICS_EXPORTS
#define GRAPHICS_API __declspec(dllexport)
#else
#define GRAPHICS_API __declspec(dllimport)
#endif

inline std::wstring AnsiToWString(const std::string& str)
{
	WCHAR buffer[512];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, buffer, 512);
	return std::wstring(buffer);
}

#ifndef THROW_IF_FAILED
#define THROW_IF_FAILED(expr, message)                               \
    do {                                                              \
        if (FAILED(expr)) {                                           \
            HRESULT hr__ = (expr);                                    \
            std::wstring wfn = AnsiToWString(__FILE__);               \
            DxException exc(hr__, message, L#expr, wfn, __LINE__);    \
            OutputDebugStringW(exc.ToString().c_str());               \
            throw exc;                                                \
        }                                                             \
    } while(false)
#endif

static constexpr DXGI_FORMAT BACK_BUFFER_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;