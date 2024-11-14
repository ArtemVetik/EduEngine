#include "../../RenderEngine/IRenderEngine.h"
#include "MeshData.h"

using namespace System::Numerics;

namespace EduEngine
{
    private ref class NativeRenderObjectWrapper
    {
    private:
        IRenderObject* m_NativeObject;

    public:
        NativeRenderObjectWrapper(MeshData^ meshData);
        ~NativeRenderObjectWrapper();
        !NativeRenderObjectWrapper();

        void SetWorldMatrix(Matrix4x4 matrix);

        IRenderObject* GetNativeObject() { return m_NativeObject; }
    };
}