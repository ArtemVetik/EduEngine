#pragma once
#include "../Graphics/PipelineStateD3D12.h"

namespace EduEngine
{
	class OpaquePass
	{
	private:
		ShaderD3D12 m_VertexShader;
		ShaderD3D12 m_PixelShader;
		RootSignatureD3D12 m_RootSignature;
		PipelineStateD3D12 m_Pso;

	public:
		OpaquePass(const RenderDeviceD3D12* device) :
			m_VertexShader(L"Shaders/color.hlsl", EDU_SHADER_TYPE_VERTEX, nullptr, "VS", "vs_5_1"),
			m_PixelShader(L"Shaders/color.hlsl", EDU_SHADER_TYPE_PIXEL, nullptr, "PS", "ps_5_1")
		{
			m_RootSignature.AddConstantBufferView(0); // pass constants

			CD3DX12_DESCRIPTOR_RANGE objConstants;
			objConstants.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
			m_RootSignature.AddDescriptorParameter(1, &objConstants); // object constants
			m_RootSignature.Build(device);

			std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			};

			m_Pso.SetInputLayout({ mInputLayout.data(), (UINT)mInputLayout.size() });
			m_Pso.SetRootSignature(&m_RootSignature);
			m_Pso.SetShader(&m_VertexShader);
			m_Pso.SetShader(&m_PixelShader);
			m_Pso.Build(device);
		}

		ID3D12RootSignature* GetD3D12RootSignature() const { return m_RootSignature.GetD3D12RootSignature(); }
		ID3D12PipelineState* GetD3D12PipelineState() const { return m_Pso.GetD3D12PipelineState(); }
	};
}