#include "RenderEngineInterop.h"

namespace EduEngine
{
	NativeRenderObjectWrapper^ RenderEngineInterop::AddObject(MeshData^ meshData)
	{
		auto renderObject = CoreSystems::GetInstance()->GetRenderEngine()->AddObject(meshData->ToNative());
		return gcnew NativeRenderObjectWrapper(renderObject);
	}

	NativeCameraWrapper^ RenderEngineInterop::CreateCamera()
	{
		auto camera = CoreSystems::GetInstance()->GetRenderEngine()->CreateCamera();
		return gcnew NativeCameraWrapper(camera);
	}
}