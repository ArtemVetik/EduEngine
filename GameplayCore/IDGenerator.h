#pragma once
#include <rpc.h>
#include <combaseapi.h>
#include "framework.h"

#pragma comment(lib, "Rpcrt4.lib")

namespace EduEngine
{
	class IDGenerator
	{
	public:
		static constexpr const GUID INVALID_GUID = {};

		static GUID Generate()
		{
			GUID guid;

			if (SUCCEEDED(CoCreateGuid(&guid)))
				return guid;

			return INVALID_GUID;
		}

		static bool CompareGUID(const GUID& guid1, const GUID& guid2)
		{
			return memcmp(&guid1, &guid2, sizeof(GUID)) == 0;
		}
	};
}