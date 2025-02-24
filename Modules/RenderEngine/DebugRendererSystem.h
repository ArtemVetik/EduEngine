#pragma once
#include "framework.h"
#include "../Graphics/RenderDeviceD3D12.h"
#include "../Graphics/ShaderD3D12.h"
#include "../Graphics/RootSignatureD3D12.h"
#include "../Graphics/PipelineStateD3D12.h"
#include "IDebugRendererSystem.h"
#include "RenderPasses.h"

namespace EduEngine
{
	class DebugRendererSystem : public IDebugRendererSystem
	{
	public:
		DebugRendererSystem(RenderDeviceD3D12* pDevice);

		void DrawBoundingBox(const DirectX::BoundingBox& box, DirectX::XMVECTOR color = DirectX::Colors::Green) override;
		void DrawBoundingBox(const DirectX::BoundingBox& box, DirectX::XMMATRIX transform, DirectX::XMVECTOR color = DirectX::Colors::Green) override;
		void DrawCapsule(const double& radius, const double& halfHeight, const DirectX::XMVECTOR& color, const DirectX::XMMATRIX& transform, int density) override;
		void DrawSphere(const double& radius, const DirectX::XMVECTOR& color, const DirectX::XMMATRIX& transform, int density) override;
		void DrawCircle(const double& radius, const DirectX::XMVECTOR& color, const DirectX::XMMATRIX& transform, int density) override;
		void DrawArrow(const DirectX::XMFLOAT3& p0, const DirectX::XMFLOAT3& p1, const DirectX::XMVECTOR& color, const DirectX::XMFLOAT3& n) override;
		void DrawPoint(const DirectX::XMFLOAT3& pos, const float& size, const DirectX::XMVECTOR& color) override;
		void DrawLine(const DirectX::XMFLOAT3& pos0, const DirectX::XMFLOAT3& pos1, const DirectX::XMVECTOR& color = DirectX::Colors::Green) override;
		void DrawLine(const DirectX::XMVECTOR& pos0, const DirectX::XMVECTOR& pos1, const DirectX::XMVECTOR& color = DirectX::Colors::Green) override;
		void DrawPlane(const DirectX::XMFLOAT4& p, const DirectX::XMVECTOR& color, float sizeWidth, float sizeNormal, bool drawCenterCross) override;
		void DrawSpotLight(const DirectX::XMFLOAT3& pos0, const DirectX::XMFLOAT3& pos1, float radius, int density, const DirectX::XMVECTOR& color = DirectX::Colors::Green) override;
		void DrawFrustrum(const DirectX::XMMATRIX& view, const DirectX::XMMATRIX& proj) override;

		void Render(DirectX::XMMATRIX, DirectX::XMFLOAT3 cameraPosition);

	private:
		void DrawInfiniteGrid(const DirectX::XMFLOAT3& cameraPosition, int gridSize, int gridLines);

	private:
		struct VertexPointColor
		{
			DirectX::XMFLOAT3 Position;
			DirectX::XMVECTOR Color;

			VertexPointColor(DirectX::XMFLOAT3 position, DirectX::XMVECTOR color) :
				Position(position),
				Color(color)
			{
			}
		};

		RenderDeviceD3D12* m_Device;
		DebugRenderPass m_RenderPass;
		std::vector<VertexPointColor> m_Lines;
	};
}