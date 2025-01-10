#include "pch.h"
#include "RayPicking.h"
#include "DirectXCollision.h"
#include <DirectXMath.h>
#include "Camera.h"
#include "RenderObject.h"

namespace EduEngine
{
	bool RayPicking::Intersect(Camera* camera, IRenderObject* renderObject, XMFLOAT2 screenSize, XMFLOAT2 screenPos, float& dist)
	{
		auto meshd3d12 = dynamic_cast<SharedMeshD3D12Impl*>(renderObject->GetMesh());

		if (!meshd3d12)
			return false;

		const aiMesh* mesh = meshd3d12->GetAiMesh();

		XMFLOAT3 minPoint(mesh->mAABB.mMin.x, mesh->mAABB.mMin.y, mesh->mAABB.mMin.z);
		XMFLOAT3 maxPoint(mesh->mAABB.mMax.x, mesh->mAABB.mMax.y, mesh->mAABB.mMax.z);

		BoundingBox meshAABB;
		BoundingBox::CreateFromPoints(meshAABB, XMLoadFloat3(&minPoint), XMLoadFloat3(&maxPoint));

		XMFLOAT4X4 P = camera->GetProjectionMatrix();

		float vx = (+2.0f * screenPos.x / screenSize.x - 1.0f) / P(0, 0);
		float vy = (-2.0f * screenPos.y / screenSize.y + 1.0f) / P(1, 1);

		XMVECTOR rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		XMVECTOR rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);

		XMMATRIX V = XMLoadFloat4x4(&camera->GetViewMatrix());
		XMMATRIX invView = XMMatrixInverse(nullptr, V);

		XMMATRIX W = renderObject->WorldMatrix;
		XMMATRIX invWorld = XMMatrixInverse(nullptr, W);

		XMMATRIX toLocal = XMMatrixMultiply(invView, invWorld);

		rayOrigin = XMVector3TransformCoord(rayOrigin, toLocal);
		rayDir = XMVector3TransformNormal(rayDir, toLocal);

		rayDir = XMVector3Normalize(rayDir);

		if (!meshAABB.Intersects(rayOrigin, rayDir, dist))
			return false;

		dist = FLT_MAX;
		XMVECTOR p0Min;
		XMVECTOR p1Min;
		XMVECTOR p2Min;

		for (size_t i = 0; i < mesh->mNumFaces; i++)
		{
			for (size_t k = 0; k < mesh->mFaces[i].mNumIndices; k += 3)
			{
				UINT i0 = mesh->mFaces[i].mIndices[k + 2];
				UINT i1 = mesh->mFaces[i].mIndices[k];
				UINT i2 = mesh->mFaces[i].mIndices[k + 1];

				auto v0 = mesh->mVertices[i0];
				auto v1 = mesh->mVertices[i1];
				auto v2 = mesh->mVertices[i2];

				XMVECTOR dxV0 = { v0.x, v0.y, v0.z };
				XMVECTOR dxV1 = { v1.x, v1.y, v1.z };
				XMVECTOR dxV2 = { v2.x, v2.y, v2.z };

				float t = 0.0f;
				if (TriangleTests::Intersects(rayOrigin, rayDir, dxV0, dxV1, dxV2, t))
				{
					if (t < dist)
					{
						dist = t;
						p0Min = dxV0;
						p1Min = dxV1;
						p2Min = dxV2;
					}
				}
			}
		}

		if (dist == FLT_MAX)
			return false;
		
		rayOrigin = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
		rayDir = XMVectorSet(vx, vy, 1.0f, 0.0f);

		rayOrigin = XMVector3TransformCoord(rayOrigin, invView);
		rayDir = XMVector3TransformNormal(rayDir, invView);

		rayDir = XMVector3Normalize(rayDir);

		p0Min = XMVector3TransformCoord(p0Min, W);
		p1Min = XMVector3TransformCoord(p1Min, W);
		p2Min = XMVector3TransformCoord(p2Min, W);

		TriangleTests::Intersects(rayOrigin, rayDir, p0Min, p1Min, p2Min, dist);
		return true;
	}

}