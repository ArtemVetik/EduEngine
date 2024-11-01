#pragma once
#include "pch.h"
#include "RenderDeviceD3D12.h"

namespace EduEngine
{
	class RootSignatureD3D12
	{
	public:
		void AddConstantBufferView(UINT shaderRegister,
								   UINT registerSpace = 0,
								   D3D12_SHADER_VISIBILITY visibility = D3D12_SHADER_VISIBILITY_ALL);

		void Build(const RenderDeviceD3D12* pDevice);

		ID3D12RootSignature* GetD3D12RootSignature() const;

	private:
		std::vector<CD3DX12_ROOT_PARAMETER> m_SlotParameters;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_Signature;
	};
}

