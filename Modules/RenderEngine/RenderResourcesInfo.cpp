#include "pch.h"
#include "RenderResourcesInfo.h"

namespace EduEngine
{
	RenderResourcesInfo::RenderResourcesInfo(IRenderEngine* renderEngine, IEditorRenderEngine* editorRenderEngine)
	{
		m_RenderEngine = dynamic_cast<RenderEngine*>(renderEngine);
		m_EditorRenderEngine = dynamic_cast<EditorRenderEngine*>(editorRenderEngine);
	}

	const char* RenderResourcesInfo::GetObjectsInfo(bool onlyUsed)
	{
		m_Oss.str("");
		m_Oss.clear();

		const char* separator = "--------------------\n";

		m_Oss << "::RUNTIME::\n\n";
		m_Oss << separator;
		m_Oss << "Render objects (" << m_RenderEngine->m_RenderObjects.size() << "):\n";
		for (auto& renderObject : m_RenderEngine->m_RenderObjects)
		{
			m_Oss << "\tPtr:\t" << renderObject.get() << "\n";
			m_Oss << "\tMesh:\t" << renderObject->GetMesh() << "\n";
			m_Oss << "\tMaterial:\t" << renderObject->GetMaterial() << "\n\n";
		}

		m_Oss << separator;
		m_Oss << "Meshes (" << m_RenderEngine->m_SharedMeshes.size() << "):\n";
		for (auto& mesh : m_RenderEngine->m_SharedMeshes)
		{
			if (onlyUsed && mesh->GetRefCount() == 0)
				continue;

			m_Oss << "\tName:\t" << mesh->GetFilePath() << "\n";
			m_Oss << "\tPtr:\t" << mesh.get() << "\n";
			m_Oss << "\tRef Count:\t" << mesh->GetRefCount() << "\n";
			m_Oss << "\tIB Ptr:\t" << mesh->GetIndexBuffer() << "\n";
			m_Oss << "\tVB Ptr:\t" << mesh->GetVertexBuffer() << "\n\n";
		}

		char texName[512];
		size_t convertedChars = 0;

		m_Oss << separator;
		m_Oss << "Textures (" << m_RenderEngine->m_Textures.size() << "):\n";
		for (auto& texture : m_RenderEngine->m_Textures)
		{
			if (onlyUsed && texture->GetRefCount() == 0)
				continue;

			wcstombs_s(&convertedChars, texName, sizeof(texName), texture->GetFilePath(), _TRUNCATE);

			m_Oss << "\tName:\t" << texName << "\n";
			m_Oss << "\tPtr:\t" << texture.get() << "\n";
			m_Oss << "\tRef Count:\t" << texture->GetRefCount() << "\n";
			m_Oss << "\tGPU Ptr:\t" << texture->GetGPUPtr() << "\n\n";
		}

		m_Oss << separator;
		m_Oss << "Materials (" << m_RenderEngine->m_Materials.size() << "):\n";
		for (auto& material : m_RenderEngine->m_Materials)
		{
			if (onlyUsed && material->GetRefCount() == 0)
				continue;

			m_Oss << "\tPtr:\t" << material.get() << "\n";
			m_Oss << "\tRef Count:\t" << material->GetRefCount() << "\n";
			m_Oss << "\tMainTex Ptr:\t" << material->GetMainTexture() << "\n\n";
		}

		m_Oss << separator;
		m_Oss << "Cameras (" << m_RenderEngine->m_Cameras.size() << "):\n";
		for (auto& camera : m_RenderEngine->m_Cameras)
		{
			m_Oss << "\tPtr:\t" << camera.get() << "\n\n";
		}

		m_Oss << "::EDITOR::\n\n";

		if (!onlyUsed || m_EditorRenderEngine->m_PreviewTex.get())
			m_Oss << "\tPreview Tex:\t" << m_EditorRenderEngine->m_PreviewTex.get() << "\n";
		if (!onlyUsed || m_EditorRenderEngine->m_PreviewMesh->GetMeshPtr())
		m_Oss << "\tPreview Mesh:\t" << m_EditorRenderEngine->m_PreviewMesh->GetMeshPtr() << "\n";

		m_Buffer = m_Oss.str();
		return m_Buffer.c_str();
	}
}