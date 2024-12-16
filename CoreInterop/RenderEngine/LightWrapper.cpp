#include "LightWrapper.h"
#include "../CoreSystems.h"

namespace EduEngine
{
	LightWrapper::LightWrapper()
	{
		m_NativeLight = CoreSystems::GetInstance()->GetRenderEngine()->CreateLight();
	}

	LightWrapper::~LightWrapper()
	{
		this->!LightWrapper();
	}

	LightWrapper::!LightWrapper()
	{
		if (!m_NativeLight)
			return;

		CoreSystems::GetInstance()->GetRenderEngine()->RemoveLight(m_NativeLight);
		m_NativeLight = nullptr;
	}

	void LightWrapper::DebugDraw()
	{
		NativeLightUnmanaged::DebugDraw(m_NativeLight);
	}

	void NativeLightUnmanaged::DebugDraw(Light* light)
	{
#define NP light->Position
#define ND light->Direction

		if (light->LightType == Light::Type::Directional)
		{
			CoreSystems::GetInstance()->GetRenderEngine()->GetDebugRender()->DrawArrow(
				light->Position,
				DirectX::XMFLOAT3(NP.x + ND.x * 5, NP.y + ND.y * 5, NP.z + ND.z * 5),
				DirectX::Colors::Green,
				DirectX::XMFLOAT3(ND.z, ND.y, ND.x)
			);
		}
		else if (light->LightType == Light::Type::Point)
		{
			auto worldMatrix = DirectX::XMMatrixTranslation(NP.x, NP.y, NP.z);
			CoreSystems::GetInstance()->GetRenderEngine()->GetDebugRender()->DrawSphere(
				light->FalloffEnd,
				DirectX::Colors::Green,
				worldMatrix,
				16
			);
		}
		else if (light->LightType == Light::Type::Spotlight)
		{
			auto point2 = DirectX::XMFLOAT3(NP.x + ND.x * light->FalloffEnd, NP.y + ND.y * light->FalloffEnd, NP.z + ND.z * light->FalloffEnd);
			CoreSystems::GetInstance()->GetRenderEngine()->GetDebugRender()->DrawSpotLight(
				light->Position,
				point2,
				light->FalloffEnd,
				16
			);
		}

#undef NP
#undef ND
	}
}