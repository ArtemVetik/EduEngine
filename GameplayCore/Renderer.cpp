#include "pch.h"
#include "Renderer.h"
#include "GameObject.h"

namespace EduEngine
{
	Renderer::Renderer(GameObject* parent) :
		Component(parent)
	{
	}

	Renderer::~Renderer()
	{
		if (m_Object)
			m_RenderEngine->RemoveObject(m_Object);
	}

	void Renderer::Initialize(IRenderEngine* renderEngine)
	{
		m_RenderEngine = renderEngine;
	}

	void Renderer::SetMesh(MeshData meshData)
	{
		m_Object = m_RenderEngine->AddObject(meshData);
	}

	void Renderer::Update()
	{
		m_Object->WorldMatrix = GetGameObject()->GetTransform()->GetWorldMatrix();
	}
}