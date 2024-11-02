#include "pch.h"
#include "DxException.h"

namespace EduEngine
{
	DxException::DxException(HRESULT hr, const std::wstring& message, const std::wstring& functionName, const std::wstring& filename, int lineNumber) :
		ErrorCode(hr),
		Message(message),
		FunctionName(functionName),
		Filename(filename),
		LineNumber(lineNumber)
	{
	}

	std::wstring DxException::ToString()const
	{
		_com_error err(ErrorCode);
		std::wstring msg = err.ErrorMessage();

		return L"DX EXCEPTION: " + FunctionName + L" failed in " + Filename + L"; line " + std::to_wstring(LineNumber) + L"; message: " + Message + L". Error: " + msg + L"\n";
	}
}