#pragma once
#include "pch.h"

namespace EduEngine
{
	class DxException
	{
	public:
		DxException() = default;
		DxException(HRESULT hr, const std::wstring& message, const std::wstring& functionName, const std::wstring& filename, int lineNumber);

		std::wstring ToString() const;

		HRESULT ErrorCode = S_OK;
		std::wstring FunctionName;
		std::wstring Message;
		std::wstring Filename;
		int LineNumber = -1;
	};
}