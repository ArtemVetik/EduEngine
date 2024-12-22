#pragma once
#include "pch.h"
#include "../Graphics.Heaps/QueueID.h"
#include "RenderDeviceD3D12.h"

namespace EduEngine
{
	class GRAPHICS_API ResourceD3D12
	{
	public:
		ResourceD3D12(RenderDeviceD3D12* pDevice, QueueID queueId) :
			m_Device(pDevice),
			m_QueueId(queueId)
		{}

		ResourceD3D12(RenderDeviceD3D12* pDevice, Microsoft::WRL::ComPtr<ID3D12Resource>& resource, QueueID queueId) :
			m_Device(pDevice),
			m_d3d12Resource(std::move(resource)),
			m_QueueId(queueId)
		{}

		virtual ~ResourceD3D12()
		{
			ReleaseResourceWrapper releaseResource;
			releaseResource.AddResource(std::move(m_d3d12Resource));

			m_Device->SafeReleaseObject(m_QueueId, std::move(releaseResource));
		}

		ID3D12Resource* GetD3D12Resource() const { return m_d3d12Resource.Get(); }

		void SetName(LPCWSTR name) { m_d3d12Resource->SetName(name); }

	protected:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_d3d12Resource;
		RenderDeviceD3D12* m_Device;
		QueueID m_QueueId;
	};
}