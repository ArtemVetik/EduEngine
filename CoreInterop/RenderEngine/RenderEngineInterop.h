#pragma once

#include "../CoreSystems.h"
#include "MeshData.h"
#include "NativeRenderObjectWrapper.h"
#include "NativeCameraWrapper.h"

using namespace System;

namespace EduEngine
{
	private ref class RenderEngineInterop
	{
	public:
		static NativeRenderObjectWrapper^ AddObject(MeshData^ meshData);
		static NativeCameraWrapper^ CreateCamera();
	};
}
