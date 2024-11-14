#pragma once

using namespace System;

namespace EduEngine
{
	public ref class TestMainInterop
	{
	public:
		static void Initialize()
		{
			Scripts::TestMain::Main();
		}

		static void Update()
		{
			GameplayFramework::Update();
		}
	};
}
