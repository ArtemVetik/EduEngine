#include "ComputePipelineStateD3D12.h"

namespace EduEngine
{
	ComputePipelineStateD3D12::ComputePipelineStateD3D12() :
		m_Device(nullptr),
		m_QueueId{}
	{
		ZeroMemory(&m_Desc, sizeof(D3D12_COMPUTE_PIPELINE_STATE_DESC));
		m_Desc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;
	}

	ComputePipelineStateD3D12::~ComputePipelineStateD3D12()
	{
		if (!m_Device)
			return;

		ReleaseResourceWrapper staleResource = {};
		staleResource.AddPageable(std::move(m_PSO));

		m_Device->SafeReleaseObject(m_QueueId, std::move(staleResource));
	}

	void ComputePipelineStateD3D12::SetRootSignature(RootSignatureD3D12* rootSignature)
	{
		m_Desc.pRootSignature = rootSignature->GetD3D12RootSignature();
	}

	void ComputePipelineStateD3D12::SetRootSignature(ID3D12RootSignature* rootSignature)
	{
		m_Desc.pRootSignature = rootSignature;
	}

	void ComputePipelineStateD3D12::SetShader(ShaderD3D12* shader)
	{
		assert(shader != nullptr);

		switch (shader->GetShaderType())
		{
		case EDU_SHADER_TYPE_COMPUTE:
			m_Desc.CS = shader->GetShaderBytecode();
			break;
		}

		assert(1);
	}

	void ComputePipelineStateD3D12::Build(RenderDeviceD3D12* pDevice, QueueID queueId)
	{
		assert(m_Device == nullptr);

		m_Device = pDevice;
		m_QueueId = queueId;
		HRESULT hr = pDevice->GetD3D12Device()->CreateComputePipelineState(&m_Desc, IID_PPV_ARGS(&m_PSO));

		THROW_IF_FAILED(hr, L"Failed to create PSO");
	}

	void ComputePipelineStateD3D12::SetName(const wchar_t* name)
	{
		m_PSO->SetName(name);
	}

	ID3D12PipelineState* ComputePipelineStateD3D12::GetD3D12PipelineState() const
	{
		return m_PSO.Get();
	}
}