#pragma once
#include "../../RenderEngine/IRenderEngine.h"

namespace EduEngine
{
	private ref class RenderResourcesInfoWrapper
	{
	public:
		RenderResourcesInfoWrapper();
		~RenderResourcesInfoWrapper();
		!RenderResourcesInfoWrapper();

		System::String^ GetObjectsInfo(bool onlyUsed);

	private:
		std::shared_ptr<IRenderResourcesInfo>* m_ObjectsStats;
	};
}