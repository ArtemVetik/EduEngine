#pragma once

#include "../CoreSystems.h"
#include "MeshData.h"
#include "NativeRenderObjectWrapper.h"

using namespace System;

namespace EduEngine
{
	public ref class RenderEngineInterop
	{
	public:
		static NativeRenderObjectWrapper^ AddObject(MeshData^ meshData);
		static void RemoveObject(NativeRenderObjectWrapper^ renderObject);
	};
}
