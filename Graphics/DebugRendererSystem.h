#pragma once
#include "framework.h"
#include "RenderDeviceD3D12.h"
#include "ShaderD3D12.h"
#include "RootSignatureD3D12.h"
#include "PipelineStateD3D12.h"

namespace EduEngine
{
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

	class GRAPHICS_API DebugRendererSystem
	{
	public:
		DebugRendererSystem(RenderDeviceD3D12* pDevice);

		void DrawBoundingBox(const DirectX::BoundingBox& box, DirectX::XMVECTOR color = DirectX::Colors::Green);
		void DrawSphere(const double& radius, const DirectX::XMVECTOR& color, const DirectX::XMMATRIX& transform, int density);
		void DrawCircle(const double& radius, const DirectX::XMVECTOR& color, const DirectX::XMMATRIX& transform, int density);
		void DrawArrow(const DirectX::XMFLOAT3& p0, const DirectX::XMFLOAT3& p1, const DirectX::XMVECTOR& color, const DirectX::XMFLOAT3& n);
		void DrawPoint(const DirectX::XMFLOAT3& pos, const float& size, const DirectX::XMVECTOR& color);
		void DrawLine(const DirectX::XMFLOAT3& pos0, const DirectX::XMFLOAT3& pos1, const DirectX::XMVECTOR& color = DirectX::Colors::Green);
		void DrawLine(const DirectX::XMVECTOR& pos0, const DirectX::XMVECTOR& pos1, const DirectX::XMVECTOR& color = DirectX::Colors::Green);
		void DrawPlane(const DirectX::XMFLOAT4& p, const DirectX::XMVECTOR& color, float sizeWidth, float sizeNormal, bool drawCenterCross);

		void Render(DirectX::XMMATRIX, DirectX::XMFLOAT3 cameraPosition);

	private:
		void DrawInfiniteGrid(const DirectX::XMFLOAT3& cameraPosition, int gridSize, int gridLines);
	
	private:
		struct CameraMVP
		{
			DirectX::XMFLOAT4X4 MVP;
			DirectX::XMFLOAT3 CamPos;
			float Padding;
		};

		struct WorldGridData
		{
			DirectX::XMFLOAT4X4 MVP;
			DirectX::XMFLOAT4 CamPos;
			float GridSize;
			float GridExtent;
			DirectX::XMFLOAT2 Padding;
		};

		struct VertexPointColor
		{
			DirectX::XMFLOAT3 Position;
			DirectX::XMVECTOR Color;

			VertexPointColor(DirectX::XMFLOAT3 position, DirectX::XMVECTOR color) :
				Position(position),
				Color(color)
			{
			}
		};

		RenderDeviceD3D12* m_Device;
		DebugRenderPass m_RenderPass;
		std::vector<VertexPointColor> m_Lines;
	};
}