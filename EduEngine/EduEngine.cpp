#include <Windows.h>
#include <crtdbg.h>
#include "../Graphics/Window.h"
#include "../Graphics/RenderEngine.h"
#include "Timer.h"
#include "GeometryGenerator.h"
#include "InputManager.h"

using namespace EduEngine;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
	PSTR cmdLine, int showCmd)
{
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

	FILE* fp;

	AllocConsole();
	freopen_s(&fp, "CONIN$", "r", stdin);
	freopen_s(&fp, "CONOUT$", "w", stdout);
	freopen_s(&fp, "CONOUT$", "w", stderr);
#endif

	GeometryGenerator geoGen;
	GeometryGenerator::MeshData boxMeshData = geoGen.CreateBox(25.0f, 25.0f, 25.0f, 1);

	EduEngine::Window window(hInstance);
	window.Initialize();

	EduEngine::InputManager::GetInstance().Initialize(hInstance, window.GetMainWindow());

	RenderDeviceD3D12* device;
	EduEngine::RenderEngine renderEngine;
	renderEngine.StartUp(window, &device);

	Camera mainCamera(window.GetClientWidth(), window.GetClientHeight());
	renderEngine.SetCamera(&mainCamera);

	auto& context = device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT);
	auto& queue = device->GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);

	context.Reset();

	auto vertexBuffer = std::make_shared<VertexBufferD3D12>(device, boxMeshData.Vertices.data(),
		sizeof(GeometryGenerator::Vertex), (UINT)boxMeshData.Vertices.size());
	auto indexBuffer = std::make_shared<IndexBufferD3D12>(device, boxMeshData.GetIndices16().data(),
		sizeof(GeometryGenerator::uint16), (UINT)boxMeshData.GetIndices16().size(), DXGI_FORMAT_R16_UINT);

	queue.CloseAndExecuteCommandContext(&context);
	device->FlushQueues();

	RenderObject r1;
	DirectX::XMStoreFloat4x4(&r1.World, DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslation(20, 0, 0.0f)));
	r1.VertexBuffer = vertexBuffer.get();
	r1.IndexBuffer = indexBuffer.get();

	RenderObject r2;
	DirectX::XMStoreFloat4x4(&r2.World, DirectX::XMMatrixTranspose(DirectX::XMMatrixTranslation(-20, 0.0f, 0.0f)));
	r2.VertexBuffer = vertexBuffer.get();
	r2.IndexBuffer = indexBuffer.get();

	RenderObject renderObjects[]
	{
		r1, r2
	};

	renderEngine.SetRenderObjects(renderObjects, 2);

	EduEngine::Timer timer = EduEngine::Timer(window.GetMainWindow(), L"EduEngine");

	MSG msg = { 0 };

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			timer.UpdateTimer();
			if (!window.IsPaused())
			{
				timer.UpdateTitleBarStats();

				InputManager::GetInstance().Update();

				DirectX::XMVECTOR cameraForward = DirectX::XMLoadFloat3(&mainCamera.GetLook());
				DirectX::XMVECTOR cameraRight = DirectX::XMLoadFloat3(&mainCamera.GetRight());
				DirectX::XMVECTOR cameraUp = DirectX::XMLoadFloat3(&mainCamera.GetUp());
				float moveSpeed = 50.0f;

				if (InputManager::GetInstance().IsKeyPressed(DIK_W))
					mainCamera.Shift(moveSpeed * cameraForward * timer.GetDeltaTime());

				if (InputManager::GetInstance().IsKeyPressed(DIK_S))
					mainCamera.Shift(moveSpeed * -cameraForward * timer.GetDeltaTime());

				if (InputManager::GetInstance().IsKeyPressed(DIK_D))
					mainCamera.Shift(moveSpeed * cameraRight * timer.GetDeltaTime());

				if (InputManager::GetInstance().IsKeyPressed(DIK_A))
					mainCamera.Shift(moveSpeed * -cameraRight * timer.GetDeltaTime());

				if (InputManager::GetInstance().IsKeyPressed(DIK_E))
					mainCamera.Shift(moveSpeed * cameraUp * timer.GetDeltaTime());
				
				if (InputManager::GetInstance().IsKeyPressed(DIK_Q))
					mainCamera.Shift(moveSpeed * -cameraUp * timer.GetDeltaTime());

				auto mouseState = InputManager::GetInstance().GetMouseState();
				
				if (mouseState.rgbButtons[1] & 0x80)
				{
					if (mouseState.lX != 0)
					{
						mainCamera.RotateY(5 * mouseState.lX * timer.GetDeltaTime());
					}
					if (mouseState.lY != 0)
					{
						mainCamera.Pitch(5 * mouseState.lY * timer.GetDeltaTime());
					}
				}

				mainCamera.Update();

				renderEngine.Update();
				renderEngine.Draw();
			}
			else
			{
				Sleep(100);
			}
		}
	}

	return 0;
}