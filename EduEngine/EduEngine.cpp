#include <Windows.h>
#include <crtdbg.h>
#include "RenderEngine.h"
#include "Timer.h"
#include "GeometryGenerator.h"
#include "InputManager.h"
#include "../GameplayCore/Scene.h"
#include "../GameplayCore/Renderer.h"

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

	Window window(hInstance);
	window.Initialize();

	InputManager::GetInstance().Initialize(hInstance, window.GetMainWindow());

	RenderDeviceD3D12* device;
	RenderEngine renderEngine;
	renderEngine.StartUp(window, &device);

	Camera mainCamera(window.GetClientWidth(), window.GetClientHeight());
	renderEngine.SetCamera(&mainCamera);

	device->GetCommandContext(D3D12_COMMAND_LIST_TYPE_DIRECT).Reset();
	
	auto vertexBuffer = std::make_shared<VertexBufferD3D12>(device, boxMeshData.Vertices.data(),
		sizeof(GeometryGenerator::Vertex), (UINT)boxMeshData.Vertices.size());
	auto indexBuffer = std::make_shared<IndexBufferD3D12>(device, boxMeshData.GetIndices16().data(),
		sizeof(GeometryGenerator::uint16), (UINT)boxMeshData.GetIndices16().size(), DXGI_FORMAT_R16_UINT);

	Scene scene;

	std::shared_ptr<GameObject> go1 = std::make_shared<GameObject>();
	auto renderer1 = go1->AddComponent<Renderer>();
	renderer1->VertexBuffer = vertexBuffer;
	renderer1->IndexBuffer = indexBuffer;
	go1->GetTransform()->SetPosition({-25, 0, 0});

	std::shared_ptr<GameObject> go2 = std::make_shared<GameObject>();
	auto renderer2 = go2->AddComponent<Renderer>();
	renderer2->VertexBuffer = vertexBuffer;
	renderer2->IndexBuffer = indexBuffer;
	go2->GetTransform()->SetPosition({+25, 0, 0});

	scene.AddGameObject(go1);
	scene.AddGameObject(go2);

	renderEngine.SetScene(&scene);

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

				for (size_t i = 0; i < scene.m_Objects.size(); i++)
					scene.m_Objects[i]->Update();

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