#pragma once

namespace EduEngine
{
	class GAMEPLAYCORE_API GameObject;

	class GAMEPLAYCORE_API Component
	{
	public:
		Component(GameObject* parent) :
			m_Parent{ parent }
		{
		}

		GameObject* GetGameObject() const { return m_Parent; }

		virtual void Update() = 0;

	private:
		GameObject* m_Parent;
	};
}