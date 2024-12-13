#include "RenderResourcesInfoWrapper.h"
#include "../CoreSystems.h"
#include "../Utils.h"

namespace EduEngine
{
	RenderResourcesInfoWrapper::RenderResourcesInfoWrapper()
	{
		auto ptr = IRenderResourcesInfo::Create(
			CoreSystems::GetInstance()->GetRenderEngine(),
			CoreSystems::GetInstance()->GetEditorRenderEngine()
		);

		m_ObjectsStats = new std::shared_ptr<IRenderResourcesInfo>(ptr);
	}

	RenderResourcesInfoWrapper::~RenderResourcesInfoWrapper()
	{
		this->!RenderResourcesInfoWrapper();
	}

	RenderResourcesInfoWrapper::!RenderResourcesInfoWrapper()
	{
		if (m_ObjectsStats)
		{
			delete m_ObjectsStats;
			m_ObjectsStats = nullptr;
		}
	}

	System::String^ RenderResourcesInfoWrapper::GetObjectsInfo(bool onlyUsed)
	{
		if (!m_ObjectsStats)
			return "null";

		return gcnew System::String(m_ObjectsStats->get()->GetObjectsInfo(onlyUsed));
	}
}