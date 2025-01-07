#pragma once
#include "framework.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <functional>

namespace EduEngine
{
	class RENDERENGINE_API RenderSettings
	{
	public:
		RenderSettings();

		void ApplyChanges();

		void QueueShadowDistanceChange(float value);
		void QueueAsyncComputeParticlesChange(bool value);
		
		float GetShadowDistance() const { return m_ShadowDistance; }
		bool GetAsyncComputeParticles() const { return m_AsyncComputeParticles; }

	private:
		void SaveToFile();
		void LoadFromFile();

	private:
		const std::string FilePath = "renderSettings.ini";

		float m_ShadowDistance = 150.0f;
		bool m_AsyncComputeParticles = false;

		std::unordered_map<std::string, std::function<void()>> m_ChangesQueue;
	};
}