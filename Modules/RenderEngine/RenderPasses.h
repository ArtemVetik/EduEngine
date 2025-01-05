#pragma once
#include "../Graphics/PipelineStateD3D12.h"
#include "../EduMath/SimpleMath.h"
#include "../Graphics/ComputePipelineStateD3D12.h"
#include "../Graphics/CommandSignatureD3D12.h"

namespace EduEngine
{
	class OpaquePass
	{
	public:
		struct ObjectConstants
		{
			DirectX::XMFLOAT4X4 World;
		};

		struct MaterialConstants
		{
			DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
			DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
			float Roughness = 0.25f;
			DirectX::XMFLOAT4X4 MatTransform = DirectX::SimpleMath::Matrix::Identity;
		};

		struct PassConstants
		{
			DirectX::XMFLOAT4X4 ViewProj;
			DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
			UINT DirectionalLightsCount = 0;
			UINT PointLightsCount = 0;
			UINT SpotLightsCount = 0;
			DirectX::XMFLOAT2 Padding;
			DirectX::XMFLOAT4 AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
		};

	private:
		ShaderD3D12 m_VertexShader;
		ShaderD3D12 m_PixelShader;
		RootSignatureD3D12 m_RootSignature;
		PipelineStateD3D12 m_Pso;

	public:
		OpaquePass(const RenderDeviceD3D12* device, D3D_SHADER_MACRO* macros = nullptr) :
			m_VertexShader(L"Shaders/ForwardOpaque.hlsl", EDU_SHADER_TYPE_VERTEX, macros, "VS", "vs_5_1"),
			m_PixelShader(L"Shaders/ForwardOpaque.hlsl", EDU_SHADER_TYPE_PIXEL, macros, "PS", "ps_5_1")
		{
			CD3DX12_DESCRIPTOR_RANGE objConstants;
			objConstants.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
			m_RootSignature.AddDescriptorParameter(1, &objConstants); // object constants

			CD3DX12_DESCRIPTOR_RANGE materialConstants;
			materialConstants.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
			m_RootSignature.AddDescriptorParameter(1, &materialConstants); // material constants

			CD3DX12_DESCRIPTOR_RANGE albedoTexture;
			albedoTexture.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
			m_RootSignature.AddDescriptorParameter(1, &albedoTexture); // diffuse map

			CD3DX12_DESCRIPTOR_RANGE lights;
			lights.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
			m_RootSignature.AddDescriptorParameter(1, &lights); // lights

			m_RootSignature.AddConstantBufferView(2); // pass constants

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
			m_Pso.SetRTVFormat(DXGI_FORMAT_R8G8B8A8_UNORM);
			m_Pso.Build(device);
		}

		ID3D12RootSignature* GetD3D12RootSignature() const { return m_RootSignature.GetD3D12RootSignature(); }
		ID3D12PipelineState* GetD3D12PipelineState() const { return m_Pso.GetD3D12PipelineState(); }
	};

	class ShadowMapPass
	{
	public:
		struct ObjectConstants
		{
			DirectX::XMFLOAT4X4 World;
		};

		struct PassConstants
		{
			DirectX::XMFLOAT4X4 ViewProj;
		};

	private:
		ShaderD3D12 m_VertexShader;
		RootSignatureD3D12 m_RootSignature;
		PipelineStateD3D12 m_Pso;

	public:
		ShadowMapPass(const RenderDeviceD3D12* device) :
			m_VertexShader(L"Shaders/Shadows.hlsl", EDU_SHADER_TYPE_VERTEX, nullptr, "VS", "vs_5_1")
		{
			CD3DX12_DESCRIPTOR_RANGE objConstants;
			objConstants.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
			m_RootSignature.AddDescriptorParameter(1, &objConstants); // object constants

			CD3DX12_DESCRIPTOR_RANGE passConstants;
			passConstants.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
			m_RootSignature.AddDescriptorParameter(1, &passConstants); // pass constants

			m_RootSignature.Build(device);

			std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			};

			D3D12_RASTERIZER_DESC rast = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			rast.DepthBias = 100000;
			rast.DepthBiasClamp = 0.0f;
			rast.SlopeScaledDepthBias = 1.0f;

			m_Pso.SetInputLayout({ mInputLayout.data(), (UINT)mInputLayout.size() });
			m_Pso.SetRootSignature(&m_RootSignature);
			m_Pso.SetRasterizerState(rast);
			m_Pso.SetShader(&m_VertexShader);
			m_Pso.Build(device);
		}

		ID3D12RootSignature* GetD3D12RootSignature() const { return m_RootSignature.GetD3D12RootSignature(); }
		ID3D12PipelineState* GetD3D12PipelineState() const { return m_Pso.GetD3D12PipelineState(); }
	};

	class GBufferPass
	{
	public:
		struct ObjectConstants
		{
			DirectX::XMFLOAT4X4 World;
		};

		struct MaterialConstants
		{
			DirectX::XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
			DirectX::XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
			float Roughness = 0.25f;
			DirectX::XMFLOAT4X4 MatTransform = DirectX::SimpleMath::Matrix::Identity;
		};

		struct PassConstants
		{
			DirectX::XMFLOAT4X4 ViewProj;
			DirectX::XMFLOAT4X4 View;
			DirectX::XMFLOAT3 EyePosW;
			UINT CascadeCount;
			DirectX::XMFLOAT4X4 CascadeTransform[4];
			float CascadeDistance[4] = { FLT_MAX, FLT_MAX, FLT_MAX, FLT_MAX };
		};

		static constexpr int GBufferCount = 3;
		static constexpr DXGI_FORMAT RtvFormats[GBufferCount] =
		{
			DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
			DXGI_FORMAT_R8G8B8A8_SNORM,
			DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		};

	private:
		ShaderD3D12 m_VertexShader;
		ShaderD3D12 m_PixelShader;
		RootSignatureD3D12 m_RootSignature;
		PipelineStateD3D12 m_Pso;

	public:
		GBufferPass(const RenderDeviceD3D12* device) :
			m_VertexShader(L"Shaders/GBuffer.hlsl", EDU_SHADER_TYPE_VERTEX, nullptr, "VS", "vs_5_1"),
			m_PixelShader(L"Shaders/GBuffer.hlsl", EDU_SHADER_TYPE_PIXEL, nullptr, "PS", "ps_5_1")
		{
			CD3DX12_DESCRIPTOR_RANGE objConstants;
			objConstants.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
			m_RootSignature.AddDescriptorParameter(1, &objConstants); // object constants

			CD3DX12_DESCRIPTOR_RANGE materialConstants;
			materialConstants.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 1);
			m_RootSignature.AddDescriptorParameter(1, &materialConstants); // material constants

			CD3DX12_DESCRIPTOR_RANGE albedoTexture;
			albedoTexture.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
			m_RootSignature.AddDescriptorParameter(1, &albedoTexture); // diffuse map

			CD3DX12_DESCRIPTOR_RANGE shadowMaps;
			shadowMaps.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 4, 1);
			m_RootSignature.AddDescriptorParameter(1, &shadowMaps); // shadow map

			m_RootSignature.AddConstantBufferView(2); // pass constants

			m_RootSignature.Build(device);

			std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 24, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 36, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			};

			m_Pso.SetRTVFormats(GBufferCount, RtvFormats);
			m_Pso.SetInputLayout({ mInputLayout.data(), (UINT)mInputLayout.size() });
			m_Pso.SetRootSignature(&m_RootSignature);
			m_Pso.SetShader(&m_VertexShader);
			m_Pso.SetShader(&m_PixelShader);
			m_Pso.Build(device);
		}

		ID3D12RootSignature* GetD3D12RootSignature() const { return m_RootSignature.GetD3D12RootSignature(); }
		ID3D12PipelineState* GetD3D12PipelineState() const { return m_Pso.GetD3D12PipelineState(); }
	};

	class DeferredLightPass
	{
	public:
		struct PassConstants
		{
			DirectX::XMFLOAT4X4 ProjInv;
			DirectX::XMFLOAT4X4 ViewInv;
			DirectX::XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
			UINT DirectionalLightsCount = 0;
			UINT PointLightsCount = 0;
			UINT SpotLightsCount = 0;
			DirectX::XMFLOAT2 Padding;
			DirectX::XMFLOAT4 ClearColor;
			DirectX::XMFLOAT4 AmbientLight = { 0.25f, 0.25f, 0.35f, 1.0f };
		};

		static constexpr DXGI_FORMAT AccumBuffFormat = DXGI_FORMAT_R16G16B16A16_FLOAT;

	private:
		ShaderD3D12 m_VertexShader;
		ShaderD3D12 m_PixelShader;
		RootSignatureD3D12 m_RootSignature;
		PipelineStateD3D12 m_Pso;

	public:
		DeferredLightPass(const RenderDeviceD3D12* device) :
			m_VertexShader(L"Shaders/DeferredLightPass.hlsl", EDU_SHADER_TYPE_VERTEX, nullptr, "VS", "vs_5_1"),
			m_PixelShader(L"Shaders/DeferredLightPass.hlsl", EDU_SHADER_TYPE_PIXEL, nullptr, "PS", "ps_5_1")
		{
			CD3DX12_DESCRIPTOR_RANGE albedoTex;
			albedoTex.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
			m_RootSignature.AddDescriptorParameter(1, &albedoTex); // albedo texture

			CD3DX12_DESCRIPTOR_RANGE normalTex;
			normalTex.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
			m_RootSignature.AddDescriptorParameter(1, &normalTex); // normal texture

			CD3DX12_DESCRIPTOR_RANGE materialTex;
			materialTex.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 2);
			m_RootSignature.AddDescriptorParameter(1, &materialTex); // material texture

			CD3DX12_DESCRIPTOR_RANGE depthTex;
			depthTex.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 3);
			m_RootSignature.AddDescriptorParameter(1, &depthTex); // depth texture

			CD3DX12_DESCRIPTOR_RANGE skybox;
			skybox.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 4);
			m_RootSignature.AddDescriptorParameter(1, &skybox); // skybox

			CD3DX12_DESCRIPTOR_RANGE lights;
			lights.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 5);
			m_RootSignature.AddDescriptorParameter(1, &lights); // lights

			m_RootSignature.AddConstantBufferView(0); // pass constants

			m_RootSignature.Build(device);

			std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			};

			auto dss = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			dss.DepthEnable = false;

			DXGI_FORMAT rtvFormats[1] = { AccumBuffFormat };

			m_Pso.SetInputLayout({ mInputLayout.data(), (UINT)mInputLayout.size() });
			m_Pso.SetRootSignature(&m_RootSignature);
			m_Pso.SetDepthStencilState(dss);
			m_Pso.SetRTVFormat(AccumBuffFormat);
			m_Pso.SetShader(&m_VertexShader);
			m_Pso.SetShader(&m_PixelShader);
			m_Pso.Build(device);
		}

		ID3D12RootSignature* GetD3D12RootSignature() const { return m_RootSignature.GetD3D12RootSignature(); }
		ID3D12PipelineState* GetD3D12PipelineState() const { return m_Pso.GetD3D12PipelineState(); }
	};

	class ToneMappingPass
	{
	private:
		ShaderD3D12 m_VertexShader;
		ShaderD3D12 m_PixelShader;
		RootSignatureD3D12 m_RootSignature;
		PipelineStateD3D12 m_Pso;

	public:
		ToneMappingPass(const RenderDeviceD3D12* device) :
			m_VertexShader(L"Shaders/ToneMapping.hlsl", EDU_SHADER_TYPE_VERTEX, nullptr, "VS", "vs_5_1"),
			m_PixelShader(L"Shaders/ToneMapping.hlsl", EDU_SHADER_TYPE_PIXEL, nullptr, "PS", "ps_5_1")
		{
			CD3DX12_DESCRIPTOR_RANGE accumTex;
			accumTex.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
			m_RootSignature.AddDescriptorParameter(1, &accumTex); // accumulation buffer

			m_RootSignature.Build(device);

			std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			};

			auto dss = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			dss.DepthEnable = false;

			m_Pso.SetInputLayout({ mInputLayout.data(), (UINT)mInputLayout.size() });
			m_Pso.SetRootSignature(&m_RootSignature);
			m_Pso.SetDepthStencilState(dss);
			m_Pso.SetShader(&m_VertexShader);
			m_Pso.SetShader(&m_PixelShader);
			m_Pso.SetRTVFormat(DXGI_FORMAT_R8G8B8A8_UNORM);
			m_Pso.Build(device);
		}

		ID3D12RootSignature* GetD3D12RootSignature() const { return m_RootSignature.GetD3D12RootSignature(); }
		ID3D12PipelineState* GetD3D12PipelineState() const { return m_Pso.GetD3D12PipelineState(); }
	};

	class SkyboxPass
	{
	public:
		struct CB
		{
			DirectX::XMFLOAT4X4 World;
			DirectX::XMFLOAT4X4 ViewProj;
			DirectX::XMFLOAT3 EyePosW;
			UINT Padding;
		};

	private:
		ShaderD3D12 m_VertexShader;
		ShaderD3D12 m_PixelShader;
		RootSignatureD3D12 m_RootSignature;
		PipelineStateD3D12 m_Pso;

	public:
		SkyboxPass(const RenderDeviceD3D12* device) :
			m_VertexShader(L"Shaders/Skybox.hlsl", EDU_SHADER_TYPE_VERTEX, nullptr, "VS", "vs_5_1"),
			m_PixelShader(L"Shaders/Skybox.hlsl", EDU_SHADER_TYPE_PIXEL, nullptr, "PS", "ps_5_1")
		{
			CD3DX12_DESCRIPTOR_RANGE skyTexCube;
			skyTexCube.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
			m_RootSignature.AddDescriptorParameter(1, &skyTexCube); // sky texture cube

			m_RootSignature.AddConstantBufferView(0); // cb

			m_RootSignature.Build(device);

			std::vector<D3D12_INPUT_ELEMENT_DESC> mInputLayout =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
			};

			auto dss = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			dss.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;

			auto rast = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			rast.CullMode = D3D12_CULL_MODE_NONE;

			m_Pso.SetInputLayout({ mInputLayout.data(), (UINT)mInputLayout.size() });
			m_Pso.SetRootSignature(&m_RootSignature);
			m_Pso.SetDepthStencilState(dss);
			m_Pso.SetRasterizerState(rast);
			m_Pso.SetShader(&m_VertexShader);
			m_Pso.SetShader(&m_PixelShader);
			m_Pso.SetRTVFormat(DXGI_FORMAT_R8G8B8A8_UNORM);
			m_Pso.Build(device);
		}

		ID3D12RootSignature* GetD3D12RootSignature() const { return m_RootSignature.GetD3D12RootSignature(); }
		ID3D12PipelineState* GetD3D12PipelineState() const { return m_Pso.GetD3D12PipelineState(); }
	};

	class DebugRenderPass
	{
	public:
		struct PassConstants
		{
			DirectX::XMFLOAT4X4 MVP;
			DirectX::XMFLOAT3 CamPos;
			float Padding;
		};

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

			auto dss = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			dss.DepthEnable = true;
			dss.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;

			m_Pso.SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE);
			m_Pso.SetBlendState(blendDesc);
			m_Pso.SetDepthStencilState(dss);
			m_Pso.SetInputLayout({ mInputLayout.data(), (UINT)mInputLayout.size() });
			m_Pso.SetRootSignature(&m_RootSignature);
			m_Pso.SetShader(&m_VertexShader);
			m_Pso.SetShader(&m_PixelShader);
			m_Pso.SetRTVFormat(DXGI_FORMAT_R8G8B8A8_UNORM);
			m_Pso.Build(device);
		}

		ID3D12RootSignature* GetD3D12RootSignature() const { return m_RootSignature.GetD3D12RootSignature(); }
		ID3D12PipelineState* GetD3D12PipelineState() const { return m_Pso.GetD3D12PipelineState(); }
	};

	class ImGuiPass
	{
	public:
		struct ProjectionMatrixBuffer
		{
			DirectX::XMFLOAT4X4 ProjectionMatrix;
		};

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
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ONE;
			blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_MAX;
			blendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
			blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
			m_Pso.SetBlendState(blendDesc);
			m_Pso.SetDepthStencilFormat(DXGI_FORMAT_UNKNOWN);

			D3D12_DEPTH_STENCIL_DESC depthStencilDesc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
			depthStencilDesc.DepthEnable = false;
			m_Pso.SetDepthStencilState(depthStencilDesc);
			m_Pso.SetRTVFormat(DXGI_FORMAT_R8G8B8A8_UNORM);

			m_Pso.Build(device);
		}

		ID3D12RootSignature* GetD3D12RootSignature() const { return m_RootSignature.GetD3D12RootSignature(); }
		ID3D12PipelineState* GetD3D12PipelineState() const { return m_Pso.GetD3D12PipelineState(); }
	};

	class ParticlesComputePass
	{
	public:
		struct PassData
		{
			DirectX::XMFLOAT4X4 MVP;
			float AspectRatio;
			float DeltaTime;
			float TotalTime;
			UINT EmitCount;
		};

		struct ParticleData
		{
			DirectX::XMFLOAT3 CenterPos;
			UINT MaxParticles;
			DirectX::XMFLOAT4 StartColor;
			DirectX::XMFLOAT4 EndColor;
			DirectX::XMFLOAT3 Velocity;
			float LifeTime;
			DirectX::XMFLOAT3 Acceleration;
			UINT Padding;
		};

	private:
		RootSignatureD3D12 m_RootSignature;
		ShaderD3D12 m_DeadListCS;
		ShaderD3D12 m_EmitCS;
		ShaderD3D12 m_UpdateCS;
		ShaderD3D12 m_CopyDrawCS;
		ComputePipelineStateD3D12 m_DeadListPSO;
		ComputePipelineStateD3D12 m_EmitPSO;
		ComputePipelineStateD3D12 m_UpdatePSO;
		ComputePipelineStateD3D12 m_CopyDrawPSO;

	public:
		ParticlesComputePass(const RenderDeviceD3D12* device) :
			m_DeadListCS(L"Shaders/ParticlesDeadListInitCS.hlsl", EDU_SHADER_TYPE_COMPUTE, nullptr, "main", "cs_5_1"),
			m_EmitCS(L"Shaders/ParticlesEmitCS.hlsl", EDU_SHADER_TYPE_COMPUTE, nullptr, "main", "cs_5_1"),
			m_UpdateCS(L"Shaders/ParticlesUpdateCS.hlsl", EDU_SHADER_TYPE_COMPUTE, nullptr, "main", "cs_5_1"),
			m_CopyDrawCS(L"Shaders/ParticlesCopyDrawCS.hlsl", EDU_SHADER_TYPE_COMPUTE, nullptr, "main", "cs_5_1")
		{
			CD3DX12_DESCRIPTOR_RANGE passData;
			passData.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
			m_RootSignature.AddDescriptorParameter(1, &passData);

			m_RootSignature.AddConstantBufferView(1);

			CD3DX12_DESCRIPTOR_RANGE particlePool;
			particlePool.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 0);
			m_RootSignature.AddDescriptorParameter(1, &particlePool);

			CD3DX12_DESCRIPTOR_RANGE deadList;
			deadList.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 1);
			m_RootSignature.AddDescriptorParameter(1, &deadList);

			CD3DX12_DESCRIPTOR_RANGE drawList;
			drawList.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 2);
			m_RootSignature.AddDescriptorParameter(1, &drawList);

			CD3DX12_DESCRIPTOR_RANGE drawArgs;
			drawArgs.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 3);
			m_RootSignature.AddDescriptorParameter(1, &drawArgs);

			CD3DX12_DESCRIPTOR_RANGE deadListCounter;
			deadListCounter.Init(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, 1, 4);
			m_RootSignature.AddDescriptorParameter(1, &deadListCounter);

			m_RootSignature.Build(device);

			m_DeadListPSO.SetRootSignature(&m_RootSignature);
			m_DeadListPSO.SetShader(&m_DeadListCS);
			m_DeadListPSO.Build(device);

			m_EmitPSO.SetRootSignature(&m_RootSignature);
			m_EmitPSO.SetShader(&m_EmitCS);
			m_EmitPSO.Build(device);

			m_UpdatePSO.SetRootSignature(&m_RootSignature);
			m_UpdatePSO.SetShader(&m_UpdateCS);
			m_UpdatePSO.Build(device);

			m_CopyDrawPSO.SetRootSignature(&m_RootSignature);
			m_CopyDrawPSO.SetShader(&m_CopyDrawCS);
			m_CopyDrawPSO.Build(device);
		}

		ID3D12RootSignature* GetD3D12RootSignature() const { return m_RootSignature.GetD3D12RootSignature(); }
		ID3D12PipelineState* GetDeadListInitPSO() const { return m_DeadListPSO.GetD3D12PipelineState(); }
		ID3D12PipelineState* GetEmitPSO() const { return m_EmitPSO.GetD3D12PipelineState(); }
		ID3D12PipelineState* GetUpdatePSO() const { return m_UpdatePSO.GetD3D12PipelineState(); }
		ID3D12PipelineState* GetCopyDrawPSO() const { return m_CopyDrawPSO.GetD3D12PipelineState(); }
	};

	class ParticlesDrawPass
	{
	private:
		ShaderD3D12 m_VertexShader;
		ShaderD3D12 m_GeometryShader;
		ShaderD3D12 m_PixelShader;
		CommandSignatureD3D12 m_CommandRootSignature;
		RootSignatureD3D12 m_RootSignature;
		PipelineStateD3D12 m_Pso;

	public:
		ParticlesDrawPass(const RenderDeviceD3D12* device) :
			m_VertexShader(L"Shaders/ParticlesDraw.hlsl", EDU_SHADER_TYPE_VERTEX, nullptr, "VS", "vs_5_1"),
			m_GeometryShader(L"Shaders/ParticlesDraw.hlsl", EDU_SHADER_TYPE_GEOMETRY, nullptr, "GS", "gs_5_1"),
			m_PixelShader(L"Shaders/ParticlesDraw.hlsl", EDU_SHADER_TYPE_PIXEL, nullptr, "PS", "ps_5_1")
		{
			CD3DX12_DESCRIPTOR_RANGE passData;
			passData.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0);
			m_RootSignature.AddDescriptorParameter(1, &passData);

			m_RootSignature.AddConstantBufferView(1);

			CD3DX12_DESCRIPTOR_RANGE particlePool;
			particlePool.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);
			m_RootSignature.AddDescriptorParameter(1, &particlePool);

			CD3DX12_DESCRIPTOR_RANGE drawList;
			drawList.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 1);
			m_RootSignature.AddDescriptorParameter(1, &drawList);

			m_RootSignature.Build(device);

			auto rastDesc = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
			rastDesc.FillMode = D3D12_FILL_MODE_SOLID;

			D3D12_DEPTH_STENCIL_DESC depthState = {};
			depthState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
			depthState.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
			depthState.DepthEnable = true;

			D3D12_RENDER_TARGET_BLEND_DESC transparencyBlendDesc = {};
			transparencyBlendDesc.BlendEnable = true;
			transparencyBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
			transparencyBlendDesc.SrcBlend = D3D12_BLEND_SRC_ALPHA;
			transparencyBlendDesc.DestBlend = D3D12_BLEND_ONE;
			transparencyBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
			transparencyBlendDesc.SrcBlendAlpha = D3D12_BLEND_ZERO;
			transparencyBlendDesc.DestBlendAlpha = D3D12_BLEND_ONE;
			transparencyBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

			m_Pso.SetRootSignature(&m_RootSignature);
			m_Pso.SetDepthStencilState(depthState);
			m_Pso.SetRasterizerState(rastDesc);
			m_Pso.SetRTBlendState(0, transparencyBlendDesc);
			m_Pso.SetPrimitiveTopology(D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT);
			m_Pso.SetShader(&m_VertexShader);
			m_Pso.SetShader(&m_GeometryShader);
			m_Pso.SetShader(&m_PixelShader);
			m_Pso.SetRTVFormat(DXGI_FORMAT_R8G8B8A8_UNORM);

			m_Pso.Build(device);

			D3D12_INDIRECT_ARGUMENT_DESC args[1];
			args[0].Type = D3D12_INDIRECT_ARGUMENT_TYPE_DRAW;

			m_CommandRootSignature.SetByteStride(9 * sizeof(UINT));
			m_CommandRootSignature.SetArguments(1, args);
			m_CommandRootSignature.Build(device);
		}

		ID3D12PipelineState* GetD3D12PipelineState() const { return m_Pso.GetD3D12PipelineState(); }
		ID3D12RootSignature* GetD3D12RootSignature() const { return m_RootSignature.GetD3D12RootSignature(); }
		ID3D12CommandSignature* GetD3D12CommandRootSignature() const { return m_CommandRootSignature.GetD3D12Signature(); }
	};
}