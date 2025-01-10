#include "RayPickingInterop.h"
#include "../../RenderEngine/RayPicking.h"

namespace EduEngine
{
    bool RayPickingInterop::Intersect(NativeRenderObjectWrapper^ renderObject,
                                      NativeCameraWrapper^       camera,
                                      Vector2                    screenSize,
                                      Vector2                    screenPos,
                                      float%                     outDist)
    {
        float dist;
        bool intersects = RayPicking::Intersect(camera->GetNative(),
                                         renderObject->GetNativeObject(),
                                         { screenSize.X, screenSize.Y },
                                         { screenPos.X, screenPos.Y},
                                         dist
        );

        outDist = dist;
        return intersects;
    }
}