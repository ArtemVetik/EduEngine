#pragma once
#include "../../RenderEngine/IRenderEngine.h"

namespace EduEngine
{
	using namespace System;
	using namespace System::Runtime::InteropServices;

	private ref class SharedMeshWrapper
	{
	public:
		SharedMeshWrapper(String^ filePath);
		~SharedMeshWrapper();

		IMesh* GetNative() { return m_NativeMesh; }

	private:
		IMesh* m_NativeMesh;
	};
}