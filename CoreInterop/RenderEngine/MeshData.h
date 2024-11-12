#pragma once
#include "../../RenderEngine/RenderEngineDefines.h"

namespace EduEngine
{
	using namespace System;
	using namespace System::Numerics;
	using namespace System::Collections::Generic;

    public ref class Vertex
    {
    public:
        System::Numerics::Vector3 Position;
        System::Numerics::Vector3 Normal;
        System::Numerics::Vector3 TangentU;
        System::Numerics::Vector2 TexC;

        Vertex() {}

        Vertex(System::Numerics::Vector3 position,
               System::Numerics::Vector3 normal,
               System::Numerics::Vector3 tangentU,
               System::Numerics::Vector2 texC) :
            Position(position),
            Normal(normal),
            TangentU(tangentU),
            TexC(texC)
        {}

        NativeVertex ToNative()
        {
            return NativeVertex(
                Position.X, Position.Y, Position.Z,
                Normal.X, Normal.Y, Normal.Z,
                TangentU.X, TangentU.Y, TangentU.Z,
                TexC.X, TexC.Y
            );
        }
    };

	public ref class MeshData
	{
	public:
        List<Vertex^>^ Vertices;
        List<uint32_t>^ Indices32;

		MeshData()
		{
			Vertices = gcnew List<Vertex^>();
			Indices32 = gcnew List<uint32_t>();
		}

        NativeMeshData ToNative()
        {
            NativeMeshData nativeMeshData;

            for each (Vertex^ vertex in Vertices)
            {
                nativeMeshData.Vertices.push_back(vertex->ToNative());
            }

            for each (uint32_t index in Indices32)
            {
                nativeMeshData.Indices32.push_back(index);
            }

            return nativeMeshData;
        }
	};
}
