#pragma once
#include "framework.h"
#include "RenderDeviceD3D12.h"

namespace EduEngine
{
	class GRAPHICS_API CommandSignatureD3D12
	{
	public:
		CommandSignatureD3D12();

		void SetByteStride(int stride);
		void SetArguments(int numDescs, const D3D12_INDIRECT_ARGUMENT_DESC* descs);

		void Build(const RenderDeviceD3D12* pDevice);

		ID3D12CommandSignature* GetD3D12Signature() const;

	private:
		D3D12_COMMAND_SIGNATURE_DESC m_Desc;
		Microsoft::WRL::ComPtr<ID3D12CommandSignature> m_Signature;
	};
}