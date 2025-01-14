#include "pch.h"
#include "RootSignatureD3D12.h"

namespace EduEngine
{
	RootSignatureD3D12::RootSignatureD3D12() :
		m_Device(nullptr),
		m_QueueId{}
	{
	}

	RootSignatureD3D12::~RootSignatureD3D12()
	{
		if (!m_Device)
			return;

		ReleaseResourceWrapper staleResource = {};
		staleResource.AddRootSignature(std::move(m_Signature));

		m_Device->SafeReleaseObject(m_QueueId, std::move(staleResource));
	}

	void RootSignatureD3D12::AddConstantBufferView(UINT					   shaderRegister,
												   UINT					   registerSpace /* = 0 */,
												   D3D12_SHADER_VISIBILITY visibility /* = D3D12_SHADER_VISIBILITY_ALL */)
	{
		CD3DX12_ROOT_PARAMETER slotParameter;
		slotParameter.InitAsConstantBufferView(shaderRegister, registerSpace, visibility);
		m_SlotParameters.emplace_back(slotParameter);
	}

	void RootSignatureD3D12::AddDescriptorParameter(UINT					  size,
													CD3DX12_DESCRIPTOR_RANGE* rangeParameters,
													D3D12_SHADER_VISIBILITY	  visibility /* = D3D12_SHADER_VISIBILITY_ALL */)
	{
		CD3DX12_ROOT_PARAMETER slotParameter;
		slotParameter.InitAsDescriptorTable(size, rangeParameters, visibility);
		m_SlotParameters.emplace_back(slotParameter);
	}

	void RootSignatureD3D12::AddConstants(UINT					  num32BitValues,
										  UINT					  shaderRegister,
										  UINT					  registerSpace,
										  D3D12_SHADER_VISIBILITY visibility /* = D3D12_SHADER_VISIBILITY_ALL */)
	{
		CD3DX12_ROOT_PARAMETER slotParameter;
		slotParameter.InitAsConstants(num32BitValues, shaderRegister, registerSpace, visibility);
		m_SlotParameters.emplace_back(slotParameter);
	}

	void RootSignatureD3D12::Build(RenderDeviceD3D12* pDevice, QueueID queueId)
	{
		assert(m_Device == nullptr);

		m_Device = pDevice;
		m_QueueId = queueId;

		auto staticSamplers = GetStaticSamplers();

		CD3DX12_ROOT_SIGNATURE_DESC rootSigDesc(m_SlotParameters.size(), m_SlotParameters.data(), (UINT)staticSamplers.size(),
			staticSamplers.data(),
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

	void RootSignatureD3D12::SetName(const wchar_t* name)
	{
		m_Signature->SetName(name);
	}

	ID3D12RootSignature* RootSignatureD3D12::GetD3D12RootSignature() const
	{
		return m_Signature.Get();
	}

	std::array<const CD3DX12_STATIC_SAMPLER_DESC, 7> RootSignatureD3D12::GetStaticSamplers()
	{
		// Applications usually only need a handful of samplers.  So just define them all up front
		// and keep them available as part of the root signature.  

		const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
			0, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

		const CD3DX12_STATIC_SAMPLER_DESC pointClamp(
			1, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

		const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
			2, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

		const CD3DX12_STATIC_SAMPLER_DESC linearClamp(
			3, // shaderRegister
			D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP); // addressW

		const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
			4, // shaderRegister
			D3D12_FILTER_ANISOTROPIC, // filter
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
			0.0f,                             // mipLODBias
			8);                               // maxAnisotropy

		const CD3DX12_STATIC_SAMPLER_DESC anisotropicClamp(
			5, // shaderRegister
			D3D12_FILTER_ANISOTROPIC, // filter
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_CLAMP,  // addressW
			0.0f,                              // mipLODBias
			8);                                // maxAnisotropy

		const CD3DX12_STATIC_SAMPLER_DESC shadow(
			6, // shaderRegister
			D3D12_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT, // filter
			D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressU
			D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressV
			D3D12_TEXTURE_ADDRESS_MODE_BORDER,  // addressW
			0.0f,                               // mipLODBias
			16,                                 // maxAnisotropy
			D3D12_COMPARISON_FUNC_LESS_EQUAL,
			D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK);

		return {
			pointWrap, pointClamp,
			linearWrap, linearClamp,
			anisotropicWrap, anisotropicClamp, shadow
		};
	}
}
