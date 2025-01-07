#pragma once
#include "../CoreSystems.h"

namespace EduEngine
{
#define SETTINGS CoreSystems::GetInstance()->GetRenderEngine()->GetRenderSettings()

	ref class RenderSettingsInterop
	{
	public:
		static property float ShadowDistance
		{
			float get() { return SETTINGS->GetShadowDistance(); }
			void set(float value) { SETTINGS->QueueShadowDistanceChange(value); }
		}

		static property bool AsyncComputeParticles
		{
			bool get() { return SETTINGS->GetAsyncComputeParticles(); }
			void set(bool value) { SETTINGS->QueueAsyncComputeParticlesChange(value); }
		}
	};

#undef SETTINGS
}