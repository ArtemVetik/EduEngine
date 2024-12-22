#pragma once
#include "d3d12.h"
#include <wrl/client.h>

namespace EduEngine
{
	class RenderEngineInternal
	{
	public:
		RenderEngineInternal()
		{
#if defined(DEBUG) || defined(_DEBUG) 
			Microsoft::WRL::ComPtr<ID3D12Debug> debugController;
			HRESULT hr = D3D12GetDebugInterface(IID_PPV_ARGS(&debugController));
			debugController->EnableDebugLayer();
#endif

			HRESULT hardwareResult = D3D12CreateDevice(
				nullptr,
				D3D_FEATURE_LEVEL_11_0,
				IID_PPV_ARGS(m_Device.GetAddressOf()));
			
			if (FAILED(hardwareResult))
				throw;
		}

		static RenderEngineInternal& GetInstance()
		{
			static RenderEngineInternal instance;
			return instance;
		}

		ID3D12Device* GetDevice() const { return m_Device.Get(); }

	private:
		Microsoft::WRL::ComPtr<ID3D12Device> m_Device;
	};
}