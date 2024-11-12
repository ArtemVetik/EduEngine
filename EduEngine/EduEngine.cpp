#include <Windows.h>
#include <crtdbg.h>
#include "Timer.h"
#include "InputManager.h"
#include "../RenderEngine/Window.h"
#include "../RenderEngine/IRenderEngine.h"
#include "../RenderEngine/Camera.h"
#include "../RenderEngine/GeometryGenerator.h"
#include "../Physics/PhysicsFactory.h"
#include "../CoreInterop/CoreSystems.h"
#include "../GameplayInterop/GameplayInterop.h"

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

	Window window(hInstance);
	window.Initialize();

	InputManager::GetInstance().Initialize(hInstance, window.GetMainWindow());

	std::shared_ptr<IRenderEngine> renderEngine = IRenderEngine::Create(window);

	Camera mainCamera(window.GetClientWidth(), window.GetClientHeight());
	renderEngine->SetCamera(&mainCamera);

	PhysicsFactory physicsFactory;
	std::shared_ptr<IPhysicsWorld> physicsWorld = physicsFactory.Create();

	EduEngine::CoreSystems s(renderEngine.get(), physicsWorld.get());

	GameplayInterop::Initialize();

	const float fixedTimeStep = 1.0f / 60.0f;
	float physixsAccumulator = 0.0f;
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
						mainCamera.RotateY(0.01f * mouseState.lX);
					}
					if (mouseState.lY != 0)
					{
						mainCamera.Pitch(0.01f * mouseState.lY);
					}
				}

				mainCamera.Update();

				physixsAccumulator += timer.GetDeltaTime();

				if (physixsAccumulator >= fixedTimeStep)
				{
					physicsWorld->Update();
					physixsAccumulator = 0.0f;
				}

				GameplayInterop::Update();

				renderEngine->Draw();
			}
			else
			{
				Sleep(100);
			}
		}
	}

	return 0;
}