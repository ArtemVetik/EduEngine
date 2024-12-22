#include "EditorRenderEngineInterop.h"
#include "../CoreSystems.h"
#include "../Utils.h"

namespace EduEngine
{
	System::Numerics::Vector2 EditorRenderEngineInterop::GetEditorSize()
	{
		auto size = CoreSystems::GetInstance()->GetEditorRenderEngine()->GetScreenSize();
		return System::Numerics::Vector2(size.x, size.y);
	}

	IntPtr EditorRenderEngineInterop::CreateImGuiEditor(IntPtr pixels, int texWidth, int texHeight, int bytesPerPixel)
	{
		return IntPtr(CoreSystems::GetInstance()->GetEditorRenderEngine()->CreateEditorImGuiUI(pixels.ToPointer(), texWidth, texHeight, bytesPerPixel));
	}

	void EditorRenderEngineInterop::UpdateImGui(IntPtr drawData)
	{
		CoreSystems::GetInstance()->GetEditorRenderEngine()->UpdateEditor(static_cast<ImDrawData*>(drawData.ToPointer()));
	}

	IntPtr EditorRenderEngineInterop::SetPreviewTexture(String^ filePath)
	{
		if (filePath == nullptr)
			return IntPtr(CoreSystems::GetInstance()->GetEditorRenderEngine()->SetPreviewTexture(nullptr));

		auto cFilePath = Utils::ToWCharPointer(filePath);
		return IntPtr(CoreSystems::GetInstance()->GetEditorRenderEngine()->SetPreviewTexture(cFilePath));
	}

	PreviewMeshInfo EditorRenderEngineInterop::SetPreviewMesh(String^ filePath)
	{
		if (filePath == nullptr)
			return PreviewMeshInfo::FromNative(CoreSystems::GetInstance()->GetEditorRenderEngine()->SetPreviewMesh(nullptr));

		auto cFilePath = Utils::ToCharPointer(filePath);
		return PreviewMeshInfo::FromNative(CoreSystems::GetInstance()->GetEditorRenderEngine()->SetPreviewMesh(cFilePath));
	}

	void EditorRenderEngineInterop::RotatePreviewMesh(Numerics::Vector3 delta)
	{
		CoreSystems::GetInstance()->GetEditorRenderEngine()->RotatePreviewMesh({ delta.X, delta.Y, delta.Z });
	}

	PreviewMeshInfo PreviewMeshInfo::FromNative(IEditorRenderEngine::PreviewMeshInfo info)
	{
		auto res = PreviewMeshInfo();
		res.TexturePtr = IntPtr(info.TexturePtr);
		res.VertexCount = info.VertexCount;
		res.IndexCount = info.IndexCount;

		return res;
	}
}