#pragma once

using namespace System;

namespace EduEngine
{
	public ref class TestMainInterop
	{
	public:
		static void Initialize()
		{
			TestMain::TestInitialize();
		}

		static void Update()
		{
			TestMain::TestUpdate();
		}
	};
}
