#include "SharedMeshWrapper.h"
#include "../CoreSystems.h"
#include "../Utils.h"

namespace EduEngine
{
	SharedMeshWrapper::SharedMeshWrapper(System::String^ filePath)
	{
		auto cFilePath = Utils::ToCharPointer(filePath);
		m_NativeMesh = CoreSystems::GetInstance()->GetRenderEngine()->CreateMesh(cFilePath);
	}

	SharedMeshWrapper::~SharedMeshWrapper()
	{
		this->!SharedMeshWrapper();
	}

	SharedMeshWrapper::!SharedMeshWrapper()
	{
		if (m_NativeMesh)
		{
			CoreSystems::GetInstance()->GetRenderEngine()->RemoveMesh(m_NativeMesh);
			m_NativeMesh = nullptr;
		}
	}

	void SharedMeshWrapper::UpdateFilePath(String^ filePath)
	{
		if (!m_NativeMesh)
			return;

		auto cFilePath = (Utils::ToCharPointer(filePath));
		m_NativeMesh->UpdateFilePath(cFilePath);
	}
}