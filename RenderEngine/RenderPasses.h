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

			CD3DX12_DESCRIPTOR_RANGE albedoTexture;
			albedoTexture.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
			m_RootSignature.AddDescriptorParameter(1, &albedoTexture);
			
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

	class DebugRenderPass
	{
	private:
		ShaderD3D12 m_VertexShader;
		ShaderD3D12 m_PixelShader;
		RootSignatureD3D12 m_RootSignature;
		PipelineStateD3D12 m_Pso;

	public:
		DebugRenderPass(const RenderDeviceD3D12* device) :
			m_VertexShader(L"Shaders/debugRender.hlsl", EDU_SHADER_TYPE_VERTEX, nullptr, "VS", "vs_5_1"),
			m_PixelShader(L"Shaders/debugRender.hlsl", EDU_SHADER_TYPE_PIXEL, nullptr, "PS", "ps_5_1")
		{
			m_RootSignature.AddConstantBufferView(0); // mvp
			m_RootSignature.Build(device);

			std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,	 0,  0,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "COLOR",	  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
			};

			D3D12_BLEND_DESC blendDesc = {};
			blendDesc.AlphaToCoverageEnable = FALSE;
			blendDesc.IndependentBlendEnable = FALSE;
			blendDesc.RenderTarget[0].BlendEnable = TRUE;
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
			blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

			m_Pso.SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
			m_Pso.SetBlendState(blendDesc);
			m_Pso.SetInputLayout({ mInputLayout.data(), (UINT)mInputLayout.size() });
			m_Pso.SetRootSignature(&m_RootSignature);
			m_Pso.SetShader(&m_VertexShader);
			m_Pso.SetShader(&m_PixelShader);
			m_Pso.Build(device);
		}

		ID3D12RootSignature* GetD3D12RootSignature() const { return m_RootSignature.GetD3D12RootSignature(); }
		ID3D12PipelineState* GetD3D12PipelineState() const { return m_Pso.GetD3D12PipelineState(); }
	};

	class ImGuiPass
	{
	private:
		ShaderD3D12 m_VertexShader;
		ShaderD3D12 m_PixelShader;
		RootSignatureD3D12 m_RootSignature;
		PipelineStateD3D12 m_Pso;

	public:
		ImGuiPass(const RenderDeviceD3D12* device) :
			m_VertexShader(L"Shaders/imgui.hlsl", EDU_SHADER_TYPE_VERTEX, nullptr, "VS", "vs_5_1"),
			m_PixelShader(L"Shaders/imgui.hlsl", EDU_SHADER_TYPE_PIXEL, nullptr, "PS", "ps_5_1")
		{
			m_RootSignature.AddConstantBufferView(0);

			CD3DX12_DESCRIPTOR_RANGE fontTexture;
			fontTexture.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
			m_RootSignature.AddDescriptorParameter(1, &fontTexture);

			m_RootSignature.Build(device);

			std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout =
			{
				{ "POSITION",	0,	DXGI_FORMAT_R32G32_FLOAT,	0,	0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD",	0,	DXGI_FORMAT_R32G32_FLOAT,	0,	8,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "COLOR",		0,	DXGI_FORMAT_R8G8B8A8_UNORM,	0, 16,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			};

			m_Pso.SetInputLayout({ mInputLayout.data(), (UINT)mInputLayout.size() });
			m_Pso.SetRootSignature(&m_RootSignature);
			m_Pso.SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE);
			m_Pso.SetShader(&m_VertexShader);
			m_Pso.SetShader(&m_PixelShader);

			D3D12_RASTERIZER_DESC rasterizerDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;
			rasterizerDesc.DepthClipEnable = false;
			m_Pso.SetRasterizerState(rasterizerDesc);

			D3D12_BLEND_DESC blendDesc = {};
			blendDesc.AlphaToCoverageEnable = false;
			blendDesc.IndependentBlendEnable = false;
			blendDesc.RenderTarget[0].BlendEnable = TRUE; 
			blendDesc.RenderTarget[0].LogicOpEnable = FALSE;
			blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA; 
			blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA; 
			blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
			blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_MAX;
			blendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
			blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
			m_Pso.SetBlendState(blendDesc);
			m_Pso.SetDepthStencilFormat(DXGI_FORMAT_UNKNOWN);

			D3D12_DEPTH_STENCIL_DESC depthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			depthStencilDesc.DepthEnable = false;
			m_Pso.SetDepthStencilState(depthStencilDesc);

			m_Pso.Build(device);
		}

		ID3D12RootSignature* GetD3D12RootSignature() const { return m_RootSignature.GetD3D12RootSignature(); }
		ID3D12PipelineState* GetD3D12PipelineState() const { return m_Pso.GetD3D12PipelineState(); }

		struct ProjectionMatrixBuffer
		{
			DirectX::XMFLOAT4X4 ProjectionMatrix;
		};
	};
}