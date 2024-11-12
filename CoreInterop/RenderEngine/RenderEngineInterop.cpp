#include "RenderEngineInterop.h"

namespace EduEngine
{
	NativeRenderObjectWrapper^ RenderEngineInterop::AddObject(MeshData^ meshData)
	{
		auto renderObject = CoreSystems::GetInstance()->GetRenderEngine()->AddObject(meshData->ToNative());
		return gcnew NativeRenderObjectWrapper(renderObject);
	}

	void RenderEngineInterop::RemoveObject(NativeRenderObjectWrapper^ renderObject)
	{
		CoreSystems::GetInstance()->GetRenderEngine()->RemoveObject(renderObject->GetNativeObject());
	}
}