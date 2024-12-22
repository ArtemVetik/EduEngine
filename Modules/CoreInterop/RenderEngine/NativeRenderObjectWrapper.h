#pragma once
#include "../../RenderEngine/IRenderEngine.h"
#include "MeshData.h"
#include "SharedMeshWrapper.h"
#include "MaterialWrapper.h"

using namespace System::Numerics;

namespace EduEngine
{
    private ref class NativeRenderObjectWrapper
    {
    private:
        IRenderObject* m_NativeObject;

    public:
        NativeRenderObjectWrapper();
        ~NativeRenderObjectWrapper();
        !NativeRenderObjectWrapper();

        void SetWorldMatrix(Matrix4x4 matrix);
        void SetMesh(SharedMeshWrapper^ mesh);
        void SetMaterial(MaterialWrapper^ material);

        IRenderObject* GetNativeObject() { return m_NativeObject; }
    };
}