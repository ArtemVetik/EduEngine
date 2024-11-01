#include "pch.h"
#include "PipelineStateD3D12.h"
#include "QueryInterface.h"
namespace EduEngine
{
	PipelineStateD3D12::PipelineStateD3D12()
	{
		ZeroMemory(&m_Desc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

		m_Desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		m_Desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		m_Desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		m_Desc.SampleMask = UINT_MAX;
		m_Desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		m_Desc.NumRenderTargets = 1;
		m_Desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
		m_Desc.SampleDesc.Count = 1;
		m_Desc.SampleDesc.Quality = 0;
		m_Desc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	}

	void PipelineStateD3D12::SetInputLayout(D3D12_INPUT_LAYOUT_DESC inputLayout)
	{
		m_Desc.InputLayout = inputLayout;
	}

	void PipelineStateD3D12::SetRootSignature(RootSignatureD3D12* rootSignature)
	{
		m_Desc.pRootSignature = rootSignature->GetD3D12RootSignature();
	}

	void PipelineStateD3D12::SetShader(ShaderD3D12* shader)
	{
		assert(shader != nullptr);

		switch (shader->GetShaderType())
		{
		case EDU_SHADER_TYPE_VERTEX:
			m_Desc.VS = shader->GetShaderBytecode();
			break;
		case EDU_SHADER_TYPE_PIXEL:
			m_Desc.PS = shader->GetShaderBytecode();
			break;
		}
	}

	void PipelineStateD3D12::Build(const RenderDeviceD3D12* pDevice)
	{
		ThrowIfFailed(pDevice->GetD3D12Device()->CreateGraphicsPipelineState(&m_Desc, IID_PPV_ARGS(&m_PSO)));
	}

	ID3D12PipelineState* PipelineStateD3D12::GetD3D12PipelineState() const
	{
		return m_PSO.Get();
	}
}