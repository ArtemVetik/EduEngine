#include "../../RenderEngine/IRenderEngine.h"

using namespace System::Numerics;

namespace EduEngine
{
    public ref class NativeRenderObjectWrapper
    {
    private:
        IRenderObject* m_NativeObject;

    public:
        NativeRenderObjectWrapper(IRenderObject* nativePtr);
        ~NativeRenderObjectWrapper();
        !NativeRenderObjectWrapper();

        void SetWorldMatrix(Matrix4x4 matrix);

        IRenderObject* GetNativeObject() { return m_NativeObject; }
    };
}