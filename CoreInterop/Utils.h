#pragma once

#include <msclr/marshal.h>

namespace EduEngine
{
	private class Utils
	{
	public:
		static const char* ToCharPointer(System::String^ str)
		{
			return (char*)(void*)Marshal::StringToHGlobalAnsi(str);
		}
	};

}