#pragma once

#include <msclr/marshal.h>

namespace EduEngine
{
	private class Utils
	{
	public:
		static const char* ToCharPointer(System::String^ str)
		{
			return (char*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalAnsi(str);
		}

		static const wchar_t* ToWCharPointer(System::String^ str)
		{
			return (wchar_t*)(void*)System::Runtime::InteropServices::Marshal::StringToHGlobalUni(str);
		}
	};

}