#pragma once
#include <cstdint>
#include <DirectXMath.h>
#include <vector>
#include "RenderEngineDefines.h"

namespace EduEngine
{
	class RENDERENGINE_API GeometryGenerator
	{
	public:
		///<summary>
		/// Creates a box centered at the origin with the given dimensions, where each
		/// face has m rows and n columns of vertices.
		///</summary>
		NativeMeshData CreateBox(float width, float height, float depth, uint32 numSubdivisions);

		///<summary>
		/// Creates a sphere centered at the origin with the given radius.  The
		/// slices and stacks parameters control the degree of tessellation.
		///</summary>
		NativeMeshData CreateSphere(float radius, uint32 sliceCount, uint32 stackCount);

		///<summary>
		/// Creates a geosphere centered at the origin with the given radius.  The
		/// depth controls the level of tessellation.
		///</summary>
		NativeMeshData CreateGeosphere(float radius, uint32 numSubdivisions);

		///<summary>
		/// Creates a cylinder parallel to the y-axis, and centered about the origin.  
		/// The bottom and top radius can vary to form various cone shapes rather than true
		// cylinders.  The slices and stacks parameters control the degree of tessellation.
		///</summary>
		NativeMeshData CreateCylinder(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount);

		///<summary>
		/// Creates an mxn grid in the xz-plane with m rows and n columns, centered
		/// at the origin with the specified width and depth.
		///</summary>
		NativeMeshData CreateGrid(float width, float depth, uint32 m, uint32 n);

		///<summary>
		/// Creates a quad aligned with the screen.  This is useful for postprocessing and screen effects.
		///</summary>
		NativeMeshData CreateQuad(float x, float y, float w, float h, float depth);

	private:
		void Subdivide(NativeMeshData& meshData);
		NativeVertex MidPoint(const NativeVertex& v0, const NativeVertex& v1);
		void BuildCylinderTopCap(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount, NativeMeshData& meshData);
		void BuildCylinderBottomCap(float bottomRadius, float topRadius, float height, uint32 sliceCount, uint32 stackCount, NativeMeshData& meshData);
	};
}