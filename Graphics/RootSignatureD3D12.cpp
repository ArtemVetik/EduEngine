#include "pch.h"
#include "RootSignatureD3D12.h"

namespace EduEngine
{
	void RootSignatureD3D12::AddConstantBufferView(UINT					   shaderRegister,
												   UINT					   registerSpace /* = 0 */,
												   D3D12_SHADER_VISIBILITY visibility /* = D3D12_SHADER_VISIBILITY_ALL */)
	{
		CD3DX12_ROOT_PARAMETER slotParameter;
		slotParameter.InitAsConstantBufferView(shaderRegister, registerSpace, visibility);
		m_SlotParameters.emplace_back(slotParameter);
	}

	void RootSignatureD3D12::Build(const RenderDeviceD3D12* pDevice)
	{
		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(m_SlotParameters.size(), m_SlotParameters.data(), 0, nullptr,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		Microsoft::WRL::ComPtr<ID3DBlob> serializedRootSig = nullptr;
		Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
		HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1,
			serializedRootSig.GetAddressOf(), errorBlob.GetAddressOf());

		if (errorBlob != nullptr)
			OutputDebugStringA((char*)errorBlob->GetBufferPointer());

		THROW_IF_FAILED(hr, L"Failed to serialize root signature");

		hr = pDevice->GetD3D12Device()->CreateRootSignature(
			0,
			serializedRootSig->GetBufferPointer(),
			serializedRootSig->GetBufferSize(),
			IID_PPV_ARGS(&m_Signature));

		THROW_IF_FAILED(hr, L"Failed to create root signature");
	}

	ID3D12RootSignature* RootSignatureD3D12::GetD3D12RootSignature() const
	{
		return m_Signature.Get();
	}
}
