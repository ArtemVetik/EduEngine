#pragma once
#include "pch.h"
#include "RingBuffer.h"

namespace EduEngine
{
    struct GRAPHICS_HEAPS_API DynamicAllocation
    {
        DynamicAllocation() = default;

        DynamicAllocation(ID3D12Resource* pBuff, size_t thisOffset, size_t thisSize) :
            pBuffer(pBuff), Offset(thisOffset), Size(thisSize) {}

        ID3D12Resource* pBuffer = nullptr;
        size_t Offset = 0;
        size_t Size = 0;
        void* CPUAddress = 0;
        D3D12_GPU_VIRTUAL_ADDRESS GPUAddress = 0;
    };

	class GRAPHICS_HEAPS_API GPURingBuffer : public RingBuffer
	{
    public:
        GPURingBuffer(size_t maxSize, ID3D12Device* pd3d12Device, bool allowCPUAccess);

        GPURingBuffer(GPURingBuffer&& rhs) noexcept;
        GPURingBuffer& operator =(GPURingBuffer&& rhs) noexcept;

        GPURingBuffer(const GPURingBuffer&) = delete;
        GPURingBuffer& operator =(GPURingBuffer&) = delete;
        
        ~GPURingBuffer();

        DynamicAllocation Allocate(size_t sizeInBytes);

    private:
        void Destroy();

        void* m_CpuVirtualAddress;
        D3D12_GPU_VIRTUAL_ADDRESS m_GpuVirtualAddress;
        Microsoft::WRL::ComPtr<ID3D12Resource> m_pBuffer;
	};
}