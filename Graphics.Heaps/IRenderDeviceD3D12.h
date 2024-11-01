#pragma once
#include "pch.h"
#include "QueueID.h"

namespace EduEngine
{
    class GRAPHICS_HEAPS_API ReleaseResourceWrapper;

    class GRAPHICS_HEAPS_API IRenderDeviceD3D12
    {
    public:
        virtual ID3D12Device* GetD3D12Device() const = 0;
        virtual void SafeReleaseObject(QueueID queueId, ReleaseResourceWrapper&& wrapper) = 0;
    };
}