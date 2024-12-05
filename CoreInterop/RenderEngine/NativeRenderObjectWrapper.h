#pragma once
#include "../../RenderEngine/IRenderEngine.h"
#include "MeshData.h"
#include "SharedMeshWrapper.h"

using namespace System::Numerics;

namespace EduEngine
{
    private ref class NativeRenderObjectWrapper
    {
    private:
        IRenderObject* m_NativeObject;

    public:
        NativeRenderObjectWrapper(SharedMeshWrapper^ mesh);
        ~NativeRenderObjectWrapper();
        !NativeRenderObjectWrapper();

        void SetWorldMatrix(Matrix4x4 matrix);

        IRenderObject* GetNativeObject() { return m_NativeObject; }
    };
}