#include "pch.h"
#include "TextureD3D12Impl.h"

namespace EduEngine
{
	TextureD3D12Impl::TextureD3D12Impl(RenderDeviceD3D12* device, const wchar_t* filePath) :
		m_Device(device),
		m_FilePath(filePath),
		m_Texture(),
		m_RefCount(0)
	{
	}

	TextureD3D12Impl::~TextureD3D12Impl()
	{
		m_RefCount = 0;
		m_Texture.reset();
	}

	void TextureD3D12Impl::UpdateFilePath(const wchar_t* filePath)
	{
		m_FilePath = filePath;
	}

	void TextureD3D12Impl::Load(D3D12_SHADER_RESOURCE_VIEW_DESC* overrideDesc)
	{
		if (m_RefCount > 0)
		{
			m_RefCount++;
			return;
		}

		m_Texture = std::make_shared<TextureD3D12>(m_Device, std::wstring(m_FilePath), QueueID::Direct);

		auto texDesc = m_Texture->GetD3D12Resource()->GetDesc();
		bool cubeMap = texDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D && texDesc.DepthOrArraySize == 6;

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

		if (overrideDesc)
		{
			srvDesc = *overrideDesc;
			srvDesc.Format = m_Texture->GetD3D12Resource()->GetDesc().Format;
		}
		else
		{
			srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
			srvDesc.Format = m_Texture->GetD3D12Resource()->GetDesc().Format;
			srvDesc.ViewDimension = cubeMap ? D3D12_SRV_DIMENSION_TEXTURECUBE : D3D12_SRV_DIMENSION_TEXTURE2D;

			if (cubeMap)
			{
				srvDesc.TextureCube.MostDetailedMip = 0;
				srvDesc.TextureCube.MipLevels = m_Texture->GetD3D12Resource()->GetDesc().MipLevels;
				srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
			}
			else
			{
				srvDesc.Texture2D.MostDetailedMip = 0;
				srvDesc.Texture2D.MipLevels = m_Texture->GetD3D12Resource()->GetDesc().MipLevels;
				srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
			}
		}

		m_Texture->CreateSRVView(&srvDesc, false);

		m_RefCount = 1;
	}

	void TextureD3D12Impl::Free()
	{
		if (m_RefCount <= 0)
			return;

		m_RefCount--;

		if (m_RefCount == 0)
		{
			m_Texture.reset();
		}
	}

	void* TextureD3D12Impl::GetGPUPtr()
	{
		if (m_Texture.get())
			return reinterpret_cast<void*>(m_Texture->GetView(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV)->GetGpuHandle().ptr);

		return nullptr;
	}
}