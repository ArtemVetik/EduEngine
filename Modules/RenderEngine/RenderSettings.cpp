#include "pch.h"
#include "RenderSettings.h"

#define DEFINE_SETTING(type, name, defaultValue) \
    type name = defaultValue;                    \

#define SERIALIZE_SETTING(file, name)     \
    file << #name << "=" << name << "\n"; \

#define DESERIALIZE_SETTING(line, name)                      \
    if (line.find(#name) == 0) {                             \
        std::string value = line.substr(line.find('=') + 1); \
        std::istringstream(value) >> name;                   \
    }                                                        \

namespace EduEngine
{
	RenderSettings::RenderSettings()
	{
		struct stat buffer;
		if (stat(FilePath.c_str(), &buffer) != 0)
			SaveToFile();

		LoadFromFile();
	}

	void RenderSettings::ApplyChanges()
	{
		if (m_ChangesQueue.empty())
			return;

		for (const auto& change : m_ChangesQueue)
			change.second();

		m_ChangesQueue.clear();
		SaveToFile();
	}

	void RenderSettings::QueueShadowDistanceChange(float value)
	{
		if (value == 0)
			return;
		
		m_ChangesQueue["shadowDistance"] = [this, value]() { m_ShadowDistance = value; };
	}

	void RenderSettings::QueueAsyncComputeParticlesChange(bool value)
	{
		m_ChangesQueue["asyncComputeParticles"] = [this, value]() { m_AsyncComputeParticles = value; };
	}

	void RenderSettings::SaveToFile()
	{
		std::ofstream file(FilePath);
		if (!file.is_open())
		{
			std::cerr << "Error: Unable to open file for writing: " << FilePath << std::endl;
			return;
		}
		SERIALIZE_SETTING(file, m_ShadowDistance)
		SERIALIZE_SETTING(file, m_AsyncComputeParticles)
		file.close();
	}

	void RenderSettings::LoadFromFile()
	{
		std::ifstream file(FilePath);
		if (!file.is_open())
		{
			std::cerr << "Error: Unable to open file for reading: " << FilePath << std::endl;
			return;
		}
		std::string line;
		while (std::getline(file, line))
		{
			DESERIALIZE_SETTING(line, m_ShadowDistance)
			DESERIALIZE_SETTING(line, m_AsyncComputeParticles)
		}
		file.close();
	}
}

#undef DEFINE_SETTING
#undef SERIALIZE_SETTING
#undef DESERIALIZE_SETTING