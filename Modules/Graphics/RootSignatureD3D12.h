#pragma once
#include "pch.h"
#include "RenderDeviceD3D12.h"

namespace EduEngine
{
	class GRAPHICS_API RootSignatureD3D12
	{
	public:
		RootSignatureD3D12();
		~RootSignatureD3D12();

		void AddConstantBufferView(UINT shaderRegister,
								   UINT registerSpace = 0,
								   D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);

		void AddDescriptorParameter(UINT					  size,
									CD3DX12_DESCRIPTOR_RANGE* rangeParameters,
									D3D12_SHADER_VISIBILITY	  visibility = D3D12_SHADER_VISIBILITY_ALL);

		void AddConstants(UINT num32BitValues,
						  UINT shaderRegister,
						  UINT registerSpace = 0,
						  D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);

		void Build(RenderDeviceD3D12* pDevice, QueueID queueId);

		void SetName(const wchar_t* name);

		ID3D12RootSignature* GetD3D12RootSignature() const;

	private:
		std::array<const CD3DX12_STATIC_SAMPLER_DESC, 7> GetStaticSamplers();

	private:
		std::vector<CD3DX12_ROOT_PARAMETER> m_SlotParameters;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_Signature;

		RenderDeviceD3D12* m_Device;
		QueueID m_QueueId;
	};
}

