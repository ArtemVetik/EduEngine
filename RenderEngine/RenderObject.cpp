#include "pch.h"
#include "RenderObject.h"

namespace EduEngine
{
	RenderObject::RenderObject(SharedMeshD3D12Impl* mesh) :
		m_Mesh(mesh)
	{
		m_Mesh->Load();
	}

	RenderObject::~RenderObject()
	{
		m_Mesh->Free();
	}
}