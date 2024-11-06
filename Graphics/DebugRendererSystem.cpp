#include "pch.h"
#include "DebugRendererSystem.h"
#include "DynamicUploadBuffer.h"

namespace EduEngine
{
	DebugRendererSystem::DebugRendererSystem(RenderDeviceD3D12* pDevice) :
		m_Device(pDevice),
		m_RenderPass(pDevice)
	{
	}

	void DebugRendererSystem::DrawBoundingBox(const DirectX::BoundingBox& box, DirectX::XMVECTOR color)
	{
		DirectX::XMFLOAT3 corners[8];

		box.GetCorners(&corners[0]);

		DrawLine(corners[0], corners[1], color);
		DrawLine(corners[1], corners[2], color);
		DrawLine(corners[2], corners[3], color);
		DrawLine(corners[3], corners[0], color);

		DrawLine(corners[4], corners[5], color);
		DrawLine(corners[5], corners[6], color);
		DrawLine(corners[6], corners[7], color);
		DrawLine(corners[7], corners[4], color);

		DrawLine(corners[0], corners[4], color);
		DrawLine(corners[1], corners[5], color);
		DrawLine(corners[2], corners[6], color);
		DrawLine(corners[3], corners[7], color);
	}

	void DebugRendererSystem::DrawSphere(const double& radius, const DirectX::XMVECTOR& color, const DirectX::XMMATRIX& transform, int density)
	{
		DrawCircle(radius, color, transform, density);
		DrawCircle(radius, color, DirectX::XMMatrixRotationX(DirectX::XM_PIDIV2) * transform, density);
		DrawCircle(radius, color, DirectX::XMMatrixRotationY(DirectX::XM_PIDIV2) * transform, density);
	}

	void DebugRendererSystem::DrawCircle(const double& radius, const DirectX::XMVECTOR& color, const DirectX::XMMATRIX& transform, int density)
	{
		double angleStep = DirectX::XM_PI * 2 / density;

		for (int i = 0; i < density; i++)
		{
			DirectX::XMFLOAT3 point0
			{
				static_cast<float>(radius * cos(angleStep * i)),
				static_cast<float>(radius * sin(angleStep * i)),
				0.0f,
			};

			DirectX::XMFLOAT3 point1
			{
				static_cast<float>(radius * cos(angleStep * (i + 1))),
				static_cast<float>(radius * sin(angleStep * (i + 1))),
				0.0f,
			};

			auto p0 = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&point0), transform);
			auto p1 = DirectX::XMVector3Transform(DirectX::XMLoadFloat3(&point1), transform);

			DirectX::XMStoreFloat3(&point0, p0);
			DirectX::XMStoreFloat3(&point1, p1);

			DrawLine(point0, point1, color);
		}
	}

	void DebugRendererSystem::DrawArrow(const DirectX::XMFLOAT3& p0, const DirectX::XMFLOAT3& p1, const DirectX::XMVECTOR& color, const DirectX::XMFLOAT3& n)
	{
		DrawLine(p0, p1, color);

		auto a = DirectX::XMVectorLerp(DirectX::XMLoadFloat3(&p0), DirectX::XMLoadFloat3(&p1), 0.85f);

		auto diff = DirectX::XMVectorSubtract(DirectX::XMLoadFloat3(&p1), DirectX::XMLoadFloat3(&p0));
		auto side = DirectX::XMVector3Cross(DirectX::XMLoadFloat3(&n), diff);
		side = DirectX::XMVectorScale(side, 0.05f);

		auto a1 = DirectX::XMVectorAdd(a, side);
		auto a2 = DirectX::XMVectorSubtract(a, side);

		DirectX::XMFLOAT3 a1f, a2f;
		DirectX::XMStoreFloat3(&a1f, a1);
		DirectX::XMStoreFloat3(&a2f, a2);

		DrawLine(a1f, p1, color);
		DrawLine(a2f, p1, color);
	}

	void DebugRendererSystem::DrawPoint(const DirectX::XMFLOAT3& pos, const float& size, const DirectX::XMVECTOR& color)
	{
		m_Lines.emplace_back(DirectX::XMFLOAT3{ pos.x + size, pos.y, pos.z }, color);
		m_Lines.emplace_back(DirectX::XMFLOAT3{ pos.x - size, pos.y, pos.z }, color);
		m_Lines.emplace_back(DirectX::XMFLOAT3{ pos.x, pos.y + size, pos.z }, color);
		m_Lines.emplace_back(DirectX::XMFLOAT3{ pos.x, pos.y - size, pos.z }, color);
		m_Lines.emplace_back(DirectX::XMFLOAT3{ pos.x, pos.y, pos.z + size }, color);
		m_Lines.emplace_back(DirectX::XMFLOAT3{ pos.x, pos.y, pos.z - size }, color);
	}

	void DebugRendererSystem::Render(DirectX::XMMATRIX mvp, DirectX::XMFLOAT3 cameraPosition)
	{
		DrawInfiniteGrid(cameraPosition, 1, 1000);

		CameraMVP cameraMVP;
		DirectX::XMStoreFloat4x4(&cameraMVP.MVP, DirectX::XMMatrixTranspose(mvp));
		cameraMVP.CamPos = cameraPosition;

		DynamicUploadBuffer passUploadBuffer(m_Device, QueueID::Direct);
		passUploadBuffer.LoadData(cameraMVP);

		auto allocation = m_Device->AllocateDynamicUploadGPUDescriptor(QueueID::Direct, m_Lines.size() * sizeof(VertexPointColor));
		memcpy(allocation.CPUAddress, m_Lines.data(), m_Lines.size() * sizeof(VertexPointColor));

		D3D12_VERTEX_BUFFER_VIEW vertexView;
		vertexView.BufferLocation = allocation.GPUAddress;
		vertexView.StrideInBytes = sizeof(VertexPointColor);
		vertexView.SizeInBytes = m_Lines.size() * sizeof(VertexPointColor);

		auto& context = m_Device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT);

		context.GetCmdList()->SetPipelineState(m_RenderPass.GetD3D12PipelineState());
		context.GetCmdList()->SetGraphicsRootSignature(m_RenderPass.GetD3D12RootSignature());

		context.GetCmdList()->IASetVertexBuffers(0, 1, &vertexView);
		context.GetCmdList()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		context.GetCmdList()->SetGraphicsRootConstantBufferView(0, passUploadBuffer.GetAllocation().GPUAddress);

		context.GetCmdList()->DrawInstanced(m_Lines.size(), 1, 0, 0);

		m_Lines.clear();
	}

	void DebugRendererSystem::DrawLine(const DirectX::XMFLOAT3& pos0, const DirectX::XMFLOAT3& pos1, const DirectX::XMVECTOR& color)
	{
		m_Lines.emplace_back(pos0, color);
		m_Lines.emplace_back(pos1, color);
	}

	void DebugRendererSystem::DrawLine(const DirectX::XMVECTOR& pos0, const DirectX::XMVECTOR& pos1, const DirectX::XMVECTOR& color)
	{
		DirectX::XMFLOAT3 p0, p1;
		DirectX::XMStoreFloat3(&p0, pos0);
		DirectX::XMStoreFloat3(&p1, pos1);
		DrawLine(p0, p1, color);
	}

	void DebugRendererSystem::DrawPlane(const DirectX::XMFLOAT4& p, const DirectX::XMVECTOR& color, float sizeWidth, float sizeNormal, bool drawCenterCross)
	{
		auto dir = DirectX::XMVectorSet(p.x, p.y, p.z, 0.0f);

		if (DirectX::XMVectorGetX(DirectX::XMVector3Length(dir)) == 0.0f)
			return;

		dir = DirectX::XMVector3Normalize(dir);

		auto up = DirectX::XMVectorSet(0, 0, 1, 0);
		auto right = DirectX::XMVector3Cross(dir, up);

		if (DirectX::XMVectorGetX(DirectX::XMVector3Length(right)) < 0.01f) {
			up = DirectX::XMVectorSet(0, 1, 0, 0);
			right = DirectX::XMVector3Cross(dir, up);
		}
		right = DirectX::XMVector3Normalize(right);

		up = DirectX::XMVector3Cross(right, dir);

		auto pos = DirectX::XMVectorScale(dir, -p.w);

		auto leftPoint = DirectX::XMVectorSubtract(pos, DirectX::XMVectorScale(right, sizeWidth));
		auto rightPoint = DirectX::XMVectorAdd(pos, DirectX::XMVectorScale(right, sizeWidth));
		auto downPoint = DirectX::XMVectorSubtract(pos, DirectX::XMVectorScale(up, sizeWidth));
		auto upPoint = DirectX::XMVectorAdd(pos, DirectX::XMVectorScale(up, sizeWidth));

		DirectX::XMVectorAdd(downPoint, DirectX::XMVectorScale(right, sizeWidth));
		DirectX::XMVectorAdd(upPoint, DirectX::XMVectorScale(right, sizeWidth));

		DrawLine(DirectX::XMVectorAdd(leftPoint, DirectX::XMVectorScale(up, sizeWidth)),
			DirectX::XMVectorAdd(rightPoint, DirectX::XMVectorScale(up, sizeWidth)),
			color);

		DrawLine(DirectX::XMVectorSubtract(leftPoint, DirectX::XMVectorScale(up, sizeWidth)),
			DirectX::XMVectorSubtract(rightPoint, DirectX::XMVectorScale(up, sizeWidth)),
			color);

		DrawLine(DirectX::XMVectorSubtract(downPoint, DirectX::XMVectorScale(right, sizeWidth)),
			DirectX::XMVectorSubtract(upPoint, DirectX::XMVectorScale(right, sizeWidth)),
			color);

		DrawLine(DirectX::XMVectorAdd(downPoint, DirectX::XMVectorScale(right, sizeWidth)),
			DirectX::XMVectorAdd(upPoint, DirectX::XMVectorScale(right, sizeWidth)),
			color);


		if (drawCenterCross) {
			DrawLine(leftPoint, rightPoint, color);
			DrawLine(downPoint, upPoint, color);
		}

		DirectX::XMFLOAT3 posF, rightF, arrowP2;
		DirectX::XMStoreFloat3(&posF, pos);
		DirectX::XMStoreFloat3(&rightF, right);
		DirectX::XMStoreFloat3(&arrowP2, DirectX::XMVectorAdd(pos, DirectX::XMVectorScale(dir, sizeNormal)));

		DrawPoint(posF, 0.5f, color);
		DrawArrow(posF, arrowP2, color, rightF);
	}

	void DebugRendererSystem::DrawInfiniteGrid(const DirectX::XMFLOAT3& cameraPosition, int gridSize, int gridLines)
	{
#define IS_MAIN_LINE(a, gridSize)						((int)a % (gridSize * 10) == 0)
#define IS_ODD_LINE(a, gridSize)						((int)(abs(a) / gridSize) % 2 == 1)
#define IS_ODD_MAIN_LINE(a, initGridSize, heightLevel)  ((int)(abs(a) / (10 * initGridSize * heightLevel)) % 2 == 1)

#define MAIN_GRID_COLOR		   DirectX::Colors::White
#define SECOND_GRID_COLOR	   DirectX::Colors::Gray
#define TRANSPARENT_GRID_COLOR DirectX::XMVECTOR{0, 0, 0, 0}

#define SET_COLOR(a, outColor, alpha, initGridSize, gridSize, heightLevel)					  \
		outColor = IS_MAIN_LINE(a, gridSize) ? MAIN_GRID_COLOR : SECOND_GRID_COLOR;			  \
		if (IS_ODD_LINE(a, gridSize))														  \
			outColor = DirectX::XMVectorLerp(color, TRANSPARENT_GRID_COLOR, alpha);			  \
		else if (IS_MAIN_LINE(a, gridSize) && IS_ODD_MAIN_LINE(a, initGridSize, heightLevel)) \
			outColor = DirectX::XMVectorLerp(color, SECOND_GRID_COLOR, alpha);				  \

		float initGridSize = gridSize;
		float heightLerp = abs(cameraPosition.y) / (gridSize * 100);
		int heightLevel = 1 << (int)heightLerp;

		gridSize *= heightLevel;

		int halfGridSize = gridSize * (gridLines / 2);

		int startX = floorf(cameraPosition.x / gridSize) * gridSize - halfGridSize;
		int startZ = floorf(cameraPosition.z / gridSize) * gridSize - halfGridSize;

		float heightAlpha = (heightLerp - (int)heightLerp);

		for (int i = 0; i <= gridLines; ++i)
		{
			int x = startX + i * gridSize;
			int z = startZ + i * gridSize;

			float alpha = heightAlpha * heightAlpha;
			DirectX::XMVECTOR color;

			SET_COLOR(x, color, alpha, initGridSize, gridSize, heightLevel);
			DrawLine(DirectX::XMFLOAT3(x, 0.0f, startZ), DirectX::XMFLOAT3(x, 0.0f, startZ + gridSize * gridLines), color);

			SET_COLOR(z, color, alpha, initGridSize, gridSize, heightLevel);
			DrawLine(DirectX::XMFLOAT3(startX, 0.0f, z), DirectX::XMFLOAT3(startX + gridSize * gridLines, 0.0f, z), color);
		}

#undef SET_COLOR
#undef TRANSPARENT_GRID_COLOR
#undef SECOND_GRID_COLOR
#undef MAIN_GRID_COLOR
#undef IS_ODD_MAIN_LINE
#undef IS_ODD_LINE
#undef IS_MAIN_LINE
	}
}