//External includes
#include <vld.h>
#include "SDL.h"
#include "SDL_surface.h"
#undef main

//Standard includes
#include <iostream>
#include <string_view>
#include <string>

//Project includes
#include "ETimer.h"
#include "ERenderer.h"
#include "RenderUtils.h"

#include "CameraMovement.h"

void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

using Scenes = std::vector<Elite::Scene>;

Scenes GenerateScenes();

int main(int const, char const* [])
{

	//Create window + surfaces
	SDL_Init(SDL_INIT_VIDEO);

	const uint32_t width = 640;
	const uint32_t height = 480;
	SDL_Window* pWindow = SDL_CreateWindow(
		"RayTracer - Kobe Vrijsen",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		width, height, 0);
	
	if (!pWindow)
		return 1;

	//Initialize "framework"
	Elite::Timer* pTimer = new Elite::Timer();
	Elite::Renderer* pRenderer = new Elite::Renderer(pWindow);

	Elite::Camera camera{};
	camera.SetScreenAspectRatio(width, height);
	//camera.SetPosition(Elite::WorldPoint{ 0.f, 0.f, 1.f });
	camera.SetPosition(Elite::WorldPoint{ 0.f, 1.f, -4.f }); //example scene
	camera.SetDirection(Elite::WorldVector{ 0.f, 0.f, 1.f }); // example scene
	camera.SetFieldOfView(float(E_PI_DIV_2)); // example scene

	Elite::RenderSettings renderSettings{};
	renderSettings.PBR = true;
	renderSettings.maxToAll = false;
	renderSettings.hardShadows = true;

	auto scenes{ GenerateScenes() };
	size_t sceneIndex{ 0 };

	Elite::Mesh bunny{};
	JL::LoadMesh(bunny, R"(lowpoly_bunny.obj)");
	scenes[2].objects += Elite::WorldObject<Elite::Mesh>{ std::move(bunny), { { 1.f, .8f, .5f }, 1.f, 1, .6f, true }, { Elite::CullMode::front } };

	//Start loop
	pTimer->Start();
	float printTimer = 0.f;
	bool isLooping = true;
	bool takeScreenshot = false;

	//Elite::Camera::Vector cameraForward{};
	//constexpr bool invertControls{ true };

	while (isLooping)
	{
		//--------- Get input events ---------
		
		SDL_Event e;
		//uint32_t buttonState{ SDL_GetMouseState(nullptr, nullptr) };
		float mouseDeltaX{};
		float mouseDeltaY{};
		float mouseWheelDelta{};

		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
			case SDL_QUIT:
				isLooping = false;
				break;

			case SDL_KEYUP:
				switch (e.key.keysym.scancode)
				{
				
				case SDL_SCANCODE_X:
					takeScreenshot = true;
					break;

				case SDL_SCANCODE_P:
					renderSettings.hardShadows ^= true;
					break;

				case SDL_SCANCODE_K:
					renderSettings.PBR ^= true;
					break;

				case SDL_SCANCODE_L:
					renderSettings.maxToAll ^= true;
					break;

				case SDL_SCANCODE_O:
					++sceneIndex;
					sceneIndex %= scenes.size();
					break;
				
				case SDL_SCANCODE_I:
					puts(
R"(

#=========================#
|                         |
|   Software ray tracer   |
|                         |
|   By    Kobe Vrijsen    |
|                         |
#=========================#

v-( Controls )
|
|   I      this help page
|
|   Arrows Move
|   WASD   Move
|   Space  Up
|   LShift Down
|
|   RMB    Pivot camera
|   LMB    Walk forward/backward
|   R+L MB Climb up/down
|   Wheel  Adjust focal length
|   + -    Adjust field of view
|
|   O      Switch scene
|   P      Toggle shadows
|   K      Toggle render mode
|   L      Toggle pixel adjustment
|
^

)"					
					);
					break;

				}
				break;

			case SDL_MOUSEMOTION:
				mouseDeltaX += e.motion.xrel;
				mouseDeltaY += e.motion.yrel;
				break;

			case SDL_MOUSEWHEEL:
				mouseWheelDelta += e.wheel.y;

			}
		}

		CameraMovement::Update(camera, pTimer->GetElapsed(), mouseDeltaX, -mouseDeltaY, mouseWheelDelta);

		//Update scene, will be moved when a better place is available.
		{
			auto y{ Elite::MakeRotationY(float(M_PI) / 4.f * pTimer->GetElapsed()) };
			for (auto& mesh : scenes[sceneIndex].objects.Get<Elite::WorldObject<Elite::Mesh>>())
			{
				mesh.Transform(y);
			}
		}

		//--------- Render ---------
		pRenderer->Render(camera, scenes[sceneIndex], renderSettings);

		//--------- Timer ---------
		pTimer->Update();
		printTimer += pTimer->GetElapsed();
		if (printTimer >= 1.f)
		{
			printTimer -= 1.f;
			std::cout << "FPS: " << pTimer->GetFPS() << '\r';
		}

		//Save screenshot after full render
		if (takeScreenshot)
		{
			if (!pRenderer->SaveBackbufferToImage())
				std::cout << "Screenshot saved!" << std::endl;
			else
				std::cout << "Something went wrong. Screenshot not saved!" << std::endl;
			takeScreenshot = false;
		}
	}
	pTimer->Stop();

	//Shutdown "framework"
	delete pRenderer;
	delete pTimer;

	ShutDown(pWindow);
	return 0;
}


Scenes GenerateScenes()
{
	using namespace Elite;

	Mesh triangle{};
	JL::LoadMesh(triangle, R"(triangle.obj)");

	return {

		Scene{
			ObjectContainer{

				WorldObject<Plane> { { { 0, -2, 0 }, { 0,  1, 0 } }, { { 1.f, 1.f, 1.f }, 1 }, {} },
				WorldObject<Plane> { { { 0, 10, 0 }, { 0, -1, 0 } }, { { 1.f, 1.f, 1.f }, 1 }, {} },
				WorldObject<Plane> { { { -5, 0, 0 }, {  1, 0, 0 } }, { { 1.f, 1.f, 1.f }, 1 }, {} },
				WorldObject<Plane> { { {  5, 0, 0 }, { -1, 0, 0 } }, { { 1.f, 1.f, 1.f }, 1 }, {} },
				WorldObject<Plane> { { { 0, 0,  5 }, { 0, 0, -1 } }, { { 1.f, 1.f, 1.f }, 1 }, {} },
				WorldObject<Plane> { { { 0, 0, -5 }, { 0, 0,  1 } }, { { 1.f, 1.f, 1.f }, 1 }, {} },

				WorldObject<Sphere>{ { { -2, 0, 0 }, 1.f / sqrtf(2) }, { { 1.f, 0.5f, .8f }, 1.f, 1, .1f, true }, {} },
				WorldObject<Sphere>{ { {  0, 0, 0 }, 1.f / sqrtf(2) }, { { 1.f, 0.5f, .8f }, 1.f, 1, .6f, true }, {} },
				WorldObject<Sphere>{ { {  2, 0, 0 }, 1.f / sqrtf(2) }, { { 1.f, 0.5f, .8f }, 1.f, 1, 1.f, true }, {} },

				WorldObject<Sphere>{ { { -2, 2.5, 0 }, 1.f / sqrtf(2) }, { { .5f, 1.f, .8f }, 1.f, 1, .1f, false }, {} },
				WorldObject<Sphere>{ { {  0, 2.5, 0 }, 1.f / sqrtf(2) }, { { .5f, 1.f, .8f }, 1.f, 1, .6f, false }, {} },
				WorldObject<Sphere>{ { {  2, 2.5, 0 }, 1.f / sqrtf(2) }, { { .5f, 1.f, .8f }, 1.f, 1, 1.f, false }, {} },

				WorldObject<Mesh>{ (triangle.Translate({ -2, 4, 0 }), triangle), { { 1.f, .8f, .5f }, 1.f, 1, .6f, true }, { CullMode::front } },
				WorldObject<Mesh>{ (triangle.Translate({  2, 0, 0 }), triangle), { { 1.f, .8f, .5f }, 1.f, 1, .6f, true }, { CullMode::both  } },
				WorldObject<Mesh>{ (triangle.Translate({  2, 0, 0 }), triangle), { { 1.f, .8f, .5f }, 1.f, 1, .6f, true }, { CullMode::back  } }

			},
			LightsourceContainer{

				WorldObject<PointLight>{ { { -1, 5, 3.f  }, 10 }, { { 1.f, 1.f, .8f } }, {} },
				WorldObject<PointLight>{ { { 1, 2, -2.5f }, 10 }, { { .8f, .8f, 1.f } }, {} },
				WorldObject<PointLight>{ { { 1, 6, -1.f  }, 10 }, { { .8f, .8f, 1.f } }, {} }

			}
		},

		Scene{
			ObjectContainer{

				WorldObject<Plane> { { { 0, -2, 0 }, { 0,  1, 0 } }, { { 1.f, 1.f, 1.f }, 1 }, {} },

				WorldObject<Sphere>{ { { -2, 0, 0 }, 1.f / sqrtf(2) }, { { 1.f, 0.5f, .8f }, 1.f, 1, .1f, true }, {} },
				WorldObject<Sphere>{ { {  0, 0, 0 }, 1.f / sqrtf(2) }, { { 1.f, 0.5f, .8f }, 1.f, 1, .6f, true }, {} },
				WorldObject<Sphere>{ { {  2, 0, 0 }, 1.f / sqrtf(2) }, { { 1.f, 0.5f, .8f }, 1.f, 1, 1.f, true }, {} },

				WorldObject<Sphere>{ { { -2, 2.5, 0 }, 1.f / sqrtf(2) }, { { .5f, 1.f, .8f }, 1.f, 1, .1f, false }, {} },
				WorldObject<Sphere>{ { {  0, 2.5, 0 }, 1.f / sqrtf(2) }, { { .5f, 1.f, .8f }, 1.f, 1, .6f, false }, {} },
				WorldObject<Sphere>{ { {  2, 2.5, 0 }, 1.f / sqrtf(2) }, { { .5f, 1.f, .8f }, 1.f, 1, 1.f, false }, {} },

				WorldObject<Mesh>{ (triangle.Translate({ -4, 0, 0 }), triangle), { { 1.f, .8f, .5f }, 1.f, 1, .6f, true }, { CullMode::front } },
				WorldObject<Mesh>{ (triangle.Translate({  2, 0, 0 }), triangle), { { 1.f, .8f, .5f }, 1.f, 1, .6f, true }, { CullMode::both  } },
				WorldObject<Mesh>{ (triangle.Translate({  2, 0, 0 }), triangle), { { 1.f, .8f, .5f }, 1.f, 1, .6f, true }, { CullMode::back  } }

			},
			LightsourceContainer{

				WorldObject<PointLight>{ { { -1, 5, 3.f  }, 10 }, { { 1.f, 1.f, .8f } }, {} },
				WorldObject<PointLight>{ { { 1, 2, -2.5f }, 10 }, { { .8f, .8f, 1.f } }, {} },
				WorldObject<PointLight>{ { { 1, 6, -1.f  }, 10 }, { { .8f, .8f, 1.f } }, {} },
				WorldObject<DirectionalLight>{ { { -.5f, -1.f, -.5f }, 1.f }, { { 1.f, 1.f, 1.f } }, {} }

			}
		},

		Scene{
			ObjectContainer{

				WorldObject<Plane> { { { 0, -2, 0 }, { 0,  1, 0 } }, { { 1.f, 1.f, 1.f }, 1 }, {} }
				// add bunny

			},
			LightsourceContainer{

				WorldObject<PointLight>{ { { -1, 5, 3.f  }, 10 }, { { 1.f, 1.f, .8f } }, {} },
				WorldObject<PointLight>{ { { 1, 2, -2.5f }, 10 }, { { .8f, .8f, 1.f } }, {} },
				WorldObject<PointLight>{ { { 1, 6, -1.f  }, 10 }, { { .8f, .8f, 1.f } }, {} },
				WorldObject<DirectionalLight>{ { { -.5f, -1.f, -.5f }, 1.f }, { { 1.f, 1.f, 1.f } }, {} }

			}
		}

	};
}