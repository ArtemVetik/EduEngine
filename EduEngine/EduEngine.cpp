#include <Windows.h>
#include <crtdbg.h>
#include "Timer.h"
#include "InputManager.h"
#include "../GameplayCore/Scene.h"
#include "../GameplayCore/Renderer.h"
#include "../GameplayCore/RigidBody.h"
#include "../RenderEngine/Window.h"
#include "../RenderEngine/IRenderEngine.h"
#include "../Physics/PhysicsFactory.h"
#include "../RenderEngine/Camera.h"
#include "../RenderEngine/GeometryGenerator.h"

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

	Scene scene;

	std::shared_ptr<GameObject> go1 = std::make_shared<GameObject>();
	go1->GetTransform()->SetPosition({ 0, 20, 0 });
	auto renderer1 = go1->AddComponent<Renderer>();
	renderer1->Initialize(renderEngine.get());
	renderer1->SetMesh(geoGen.CreateBox(1.0f, 1.0f, 1.0f, 1));
	auto rigidBody1 = go1->AddComponent<RigidBody>();
	rigidBody1->Initialize(physicsWorld.get(), renderEngine->GetDebugRender());
	rigidBody1->SetGeometry(ColliderShape(0.6f, 0.6f, 0.6f));

	std::shared_ptr<GameObject> go2 = std::make_shared<GameObject>();
	auto renderer2 = go2->AddComponent<Renderer>();
	go2->GetTransform()->SetPosition({ 0, 30, 0 });
	renderer2->Initialize(renderEngine.get());
	renderer2->SetMesh(geoGen.CreateSphere(1.0f, 16, 16));
	auto rigidBody2 = go2->AddComponent<RigidBody>();
	rigidBody2->Initialize(physicsWorld.get(), renderEngine->GetDebugRender());
	rigidBody2->SetGeometry(ColliderShape(1));

	scene.AddGameObject(go1);
	scene.AddGameObject(go2);

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

				if (InputManager::GetInstance().IsKeyPressed(DIK_I))
					rigidBody1->SetStatic(true);
				if (InputManager::GetInstance().IsKeyPressed(DIK_O))
					rigidBody1->SetStatic(false);
				if (InputManager::GetInstance().IsKeyPressed(DIK_SPACE))
					rigidBody1->AddForce({0, 0.2f, 0}, ForceMode::IMPULSE);

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

				for (size_t i = 0; i < scene.m_Objects.size(); i++)
					scene.m_Objects[i]->Update();

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