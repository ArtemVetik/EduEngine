#pragma once
#include "NativeRenderObjectWrapper.h"
#include "NativeCameraWrapper.h"

namespace EduEngine
{
	using namespace System::Numerics;

	private ref class RayPickingInterop
	{
	public:
		static bool Intersect(NativeRenderObjectWrapper^ renderObject,
							  NativeCameraWrapper^		 camera,
							  Vector2					 screenSize,
							  Vector2					 screenPos,
							  [Out] float%					 outDist);
	};
}