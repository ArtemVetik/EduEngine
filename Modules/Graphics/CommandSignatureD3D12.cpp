#include "CommandSignatureD3D12.h"

namespace EduEngine
{
	CommandSignatureD3D12::CommandSignatureD3D12() :
		m_Desc{},
		m_Device(nullptr)
	{
	}

	CommandSignatureD3D12::~CommandSignatureD3D12()
	{
		if (!m_Device)
			return;

		ReleaseResourceWrapper staleResource = {};
		staleResource.AddPageable(std::move(m_Signature));

		m_Device->SafeReleaseObject(QueueID::Direct, std::move(staleResource));
	}

	void CommandSignatureD3D12::SetByteStride(int stride)
	{
		m_Desc.ByteStride = stride;
	}

	void CommandSignatureD3D12::SetArguments(int numDescs, const D3D12_INDIRECT_ARGUMENT_DESC* descs)
	{
		m_Desc.NumArgumentDescs = numDescs;
		m_Desc.pArgumentDescs = descs;
	}

	void CommandSignatureD3D12::Build(RenderDeviceD3D12* pDevice)
	{
		assert(m_Device == nullptr);

		m_Device = pDevice;
		pDevice->GetD3D12Device()->CreateCommandSignature(&m_Desc, nullptr, IID_PPV_ARGS(m_Signature.GetAddressOf()));
	}

	void CommandSignatureD3D12::SetName(const wchar_t* name) 
	{
		m_Signature->SetName(name);
	}

	ID3D12CommandSignature* CommandSignatureD3D12::GetD3D12Signature() const
	{
		return m_Signature.Get();
	}
}