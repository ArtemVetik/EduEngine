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
		CoreSystems::GetInstance()->GetRenderEngine()->RemoveMesh(m_NativeMesh);
		m_NativeMesh = nullptr;
	}
}