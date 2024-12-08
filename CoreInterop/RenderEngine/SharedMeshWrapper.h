#pragma once
#include "../../RenderEngine/IRenderEngine.h"
#include <memory>
#include <gcroot.h>
namespace EduEngine
{
	using namespace System;
	using namespace System::Runtime::InteropServices;

	private ref class SharedMeshWrapper
	{
	public:
		SharedMeshWrapper(String^ filePath);
		~SharedMeshWrapper();
		!SharedMeshWrapper();

		int GetVertexCount() { return m_NativeMesh->GetVertexCount(); }
		int GetIndexCount() { return m_NativeMesh->GetIndexCount(); }

		IMesh* GetNative() { return m_NativeMesh; }

	private:
		IMesh* m_NativeMesh;
	};
}