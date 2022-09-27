#include "pch.h"

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
#include "Elite/ETimer.h"
#include "ERenderer.h"
#include "RenderUtils.h"
#include "JL/Devel.h"

void ShutDown(SDL_Window* pWindow)
{
	SDL_DestroyWindow(pWindow);
	SDL_Quit();
}

Elite::Scene LoadScene(ID3D11Device*);
void SetupScene(Elite::Scene&);

int errorhandling(char const* msg, int const err);

int main(int const, char const* [])
{

	try
	{

		puts("\n\tRasterizer - Kobe Vrijsen\n\n");
		puts("For more info and controls, press 'I'.");
		puts("\n");

		//Create window + surfaces
		SDL_Init(SDL_INIT_VIDEO);
	
		constexpr bool big  = !DEBUGMODE && false;	// show bigger window
		constexpr bool tiny = false;				// show tiny window
	
		const uint32_t width  = big ? 2560 / 2 : tiny ? 100 : 640;
		const uint32_t height = big ? 1440 / 2 : tiny ? 75  : 480;
		SDL_Window* pWindow = SDL_CreateWindow(
			"Rasterizer - Kobe Vrijsen",
			SDL_WINDOWPOS_UNDEFINED,
			SDL_WINDOWPOS_UNDEFINED,
			width, height, 0);
		
		if (!pWindow)
			return 1;
	
		//Initialize "framework"
		Elite::Timer timer{};
		Elite::Renderer renderer{ pWindow };
	
		Elite::Camera camera{};
		camera.SetScreenAspectRatio(width, height);
		camera.SetPosition (Elite::WorldPoint { 0.f, 0.f, 50.f });
		camera.SetDirection(Elite::WorldVector{ 0.f, 0.f, -1.f });
		camera.SetFieldOfView(float(E_PI_DIV_2));
		
		bool freecam = false;
		Elite::RenderOptions renderOptions{};
	
		auto scene{ LoadScene(renderer.GetDevice()) };
		//SetupScene(scene);
	
		//Start loop
		timer.Start();
		float printTimer = 0.f;
		bool isLooping = true;
		bool takeScreenshot = false;
	
		while (isLooping)
		{
			float deltaT = timer.GetElapsed();
	
			//--------- Get input events ---------
			
			SDL_Event e;
			struct {
				float dX{};
				float dY{};
				float dWheel{};
			} mouse{};
	
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
						puts("> Screenshot");
						break;

					case SDL_SCANCODE_R:
						renderOptions.mode ^= true;
						puts("> Render mode changed");
						break;

					case SDL_SCANCODE_V:
						freecam ^= true;
						puts("> Freecam toggled");
						break;
	
					case SDL_SCANCODE_M:
						renderOptions.renderDepth ^= true;
						puts("> Depth buffer");
						break;
	
					case SDL_SCANCODE_N:
						renderOptions.renderNormal ^= true;
						puts("> Normals toggled");
						break;
	
					case SDL_SCANCODE_L:
						renderOptions.renderSpecular ^= true;
						puts("> Specular toggled");
						break;

					case SDL_SCANCODE_C:
						switch (renderOptions.culling)
						{
						case SHADER::Culling::FRONT:
							renderOptions.culling = SHADER::Culling::BACK;
							puts("> Back culling");
							break;

						case SHADER::Culling::BACK:
							renderOptions.culling = SHADER::Culling::BOTH;
							puts("> Both culling");
							break;

						case SHADER::Culling::BOTH:
							renderOptions.culling = SHADER::Culling::FRONT;
							puts("> Front culling");
							break;
						}
						break;

					case SDL_SCANCODE_T:
						renderOptions.transparency ^= true;
						puts("> transparency toggled");
						break;

					case SDL_SCANCODE_F:
						switch (renderOptions.sampling)
						{
						case SHADER::Sampler::POINT:
							renderOptions.sampling = SHADER::Sampler::LINEAR;
							puts("> Linear sampling");
							break;

						case SHADER::Sampler::LINEAR:
							renderOptions.sampling = SHADER::Sampler::ANISOTROPIC;
							puts("> Anisotrpic sampling");
							break;

						case SHADER::Sampler::ANISOTROPIC:
							renderOptions.sampling = SHADER::Sampler::POINT;
							puts("> Point sampling");
							break;
						}
						break;
					
					case SDL_SCANCODE_I:
						std::cout <<
R"(

#=========================#
|                         |
|       Rasterizer        |
|                         |
|   By    Kobe Vrijsen    |
|                         |
#=========================#

v-( Controls )
|
|   I      this help page
|
|   V      Toggle freecam
|
|   Arrows Move
|   WASD   Move
|   Space  Up
|   LShift Down
|
|   RMB    Pivot camera
|   LMB    Walk forward/backward
|   R+L MB Climb up/down
|   + -    Adjust field of view
|   Wheel  Adjust focal length
|
|   R      Change render mode
|
|   C      Change cull mode
|   L      Toggle specular light
|   N      Toggle normal map
|   M      Render depth buffer
|
|   T      Toggle transparancy
|   F      Change texture sampling
|
^

)"					
						;
						break;
	
					}
					break;
	
				case SDL_MOUSEMOTION:
					mouse.dX += e.motion.xrel;
					mouse.dY += e.motion.yrel;
					break;
	
				case SDL_MOUSEWHEEL:
					mouse.dWheel += e.wheel.y;
	
				}
			}
	
			//Elite::WorldPoint const pivot = Elite::CalculateCenter(scene);
	
			deltaT = timer.GetElapsed();
			JL::CameraMovement::Update(camera, deltaT, mouse.dX, -mouse.dY, mouse.dWheel, freecam);
	
			//--------- Render ---------
			renderer.Render(camera, scene, renderOptions);
	
			//--------- Timer ---------
			timer.Update();
			printTimer += deltaT;
			if (printTimer >= 1.f)
			{
				printTimer -= 1.f;
				std::cout << " FPS: " << timer.GetFPS() << "    \r";
			}
	
			//Save screenshot after full render
			if (takeScreenshot)
			{
				if (!renderer.SaveBackbufferToImage())
					std::cout << "Screenshot saved!" << std::endl;
				else
					std::cout << "Something went wrong. Screenshot not saved!" << std::endl;
				takeScreenshot = false;
			}
		}
	
		timer.Stop();
	
		//Shutdown "framework"
		ShutDown(pWindow);
	
		return 0;
	
	}
	catch (const std::exception& e)
	{
		return errorhandling(e.what(), -1);
	}
	catch (char const* msg)
	{
		return errorhandling(msg, -1);
	}
	catch (int err)
	{
		return errorhandling("See error code", err);
	}
	catch (...)
	{
		puts("[FATAL] Unknown exception was caught!");
		return -2;
	}

}

int errorhandling(char const* msg, int const err)
{
	puts("[FATAL] Exception caught!");
	puts(msg);
	return err;
}

Elite::Scene LoadScene(ID3D11Device* pDevice)
{
	using namespace Elite;

	//auto tuktukObj       = JL::OBJ_Load("Resources/tuktuk.obj");
	auto vehicleObj      = JL::OBJ_Load("Resources/vehicle.obj");
	auto fireObj         = JL::OBJ_Load("Resources/fireFX.obj");

	//auto gridSurface     = SurfaceManager::GenID("Resources/uv_grid_2.png");

	//auto tuktukDiffuse   = SurfaceManager::GenID("Resources/tuktuk.png");

	auto vehicleDiffuse  = SurfaceManager::GenID("Resources/vehicle_diffuse.png");
	auto vehicleSpecular = SurfaceManager::GenID("Resources/vehicle_specular.png");
	auto vehicleGloss    = SurfaceManager::GenID("Resources/vehicle_gloss.png");
	auto vehicleNormal   = SurfaceManager::GenID("Resources/vehicle_normal.png");

	auto fireDiffuse     = SurfaceManager::GenID("Resources/fireFX_diffuse.png");

	return
	{
		ObjectContainer
		{

			//Object<Triangle>
			//{
			//	/*Triangle*/
			//	{
			//		Vertex{ {  0, sqrtf(3) , 0 }, { UVPoint{ 0.5f , sqrtf(3) / 2 } } },
			//		Vertex{ { -1, 0        , 0 }, { UVPoint{ 0    , 0            } } },
			//		Vertex{ {  1, 0        , 0 }, { UVPoint{ 1    , 0            } } }
			//	},
			//	{
			//		gridSurface
			//	}
			//},

			//Object<Triangle>
			//{
			//	/*Triangle*/
			//	{
			//		Vertex{ {  0.5 , sqrtf(3) , 0  }, { { 0.5f , sqrtf(3) / 2 } } },
			//		Vertex{ { -0.5 , 0        , -1 }, { { 0    , 0            } } },
			//		Vertex{ {  1.5 , 0        , 1  }, { { 1    , 0            } } }
			//	},
			//	{
			//		gridSurface
			//	}
			//},

			//Object<List>
			//{
			//	List
			//	{
			//		{
			//			Vertex{ {-1, 0, -1}, { { 0, 0 } } },
			//			Vertex{ { 0, 0, -1}, { {.5, 0 } } },
			//			Vertex{ { 1, 0, -1}, { { 1, 0 } } },
			//			Vertex{ {-1,-1, -1}, { { 0,.5 } } },
			//			Vertex{ { 0,-1, -1}, { {.5,.5 } } },
			//			Vertex{ { 1,-1, -1}, { { 1,.5 } } },
			//			Vertex{ {-1,-2, -1}, { { 0, 1 } } },
			//			Vertex{ { 0,-2, -1}, { {.5, 1 } } },
			//			Vertex{ { 1,-2, -1}, { { 1, 1 } } }
			//		},
			//		{
			//			{ 0, 1, 3 },
			//			{ 3, 1, 4 },
			//			{ 1, 2, 4 },
			//			{ 4, 2, 5 },
			//			{ 3, 4, 6 },
			//			{ 6, 4, 7 },
			//			{ 4, 5, 7 },
			//			{ 7, 5, 8 }
			//		}
			//	},
			//	{
			//		yeboiSurface
			//	}
			//},

			//Object<Strip>
			//{
			//	/*Strip*/
			//	{
			//		{
			//			Vertex{ {-1, 0, -1}, { { 0, 0 } } },
			//			Vertex{ { 0, 0, -1}, { {.5, 0 } } },
			//			Vertex{ { 1, 0, -1}, { { 1, 0 } } },
			//			Vertex{ {-1,-1, -1}, { { 0,.5 } } },
			//			Vertex{ { 0,-1, -1}, { {.5,.5 } } },
			//			Vertex{ { 1,-1, -1}, { { 1,.5 } } },
			//			Vertex{ {-1,-2, -1}, { { 0, 1 } } },
			//			Vertex{ { 0,-2, -1}, { {.5, 1 } } },
			//			Vertex{ { 1,-2, -1}, { { 1, 1 } } }
			//		},
			//		{
			//			0, 3, 1, 4, 2, 5, 5, 3, 3, 6, 4, 7, 5, 8
			//		}
			//	},
			//	{
			//		gridSurface
			//	}
			//},

			//Object<Strip>
			//{
			//	{
			//		{
			//			{ { -10,  10 }, { { 0, 0 } } },
			//			{ {  10,  10 }, { { 0, 1 } } },
			//			{ {  10, -10 }, { { 1, 1 } } },
			//			{ { -10, -10 }, { { 1, 0 } } }
			//		},
			//		{
			//			0, 1, 3, 2
			//		}
			//	},
			//	{
			//		globeSurface
			//	}
			//}

			Object<List>
			{
				List{ pDevice, ConstructMesh(vehicleObj), false },
				{
					vehicleDiffuse,
					vehicleSpecular,
					vehicleGloss,
					vehicleNormal
				}
			},

			//Object<List>
			//{
			//	List{ pDevice, ConstructMesh(tuktukObj) },
			//	{
			//		tuktukDiffuse
			//	}
			//}

			Object<List>
			{
				List{ pDevice, ConstructMesh(fireObj), true },
				{
					fireDiffuse
				}
			}

		},
		{
			pDevice,
			{
				//gridSurface,
				//tuktukDiffuse,
				vehicleDiffuse,
				vehicleSpecular,
				vehicleGloss,
				vehicleNormal,
				fireDiffuse
			}
		}
	};

}

void SetupScene(Elite::Scene& scene)
{

	//using namespace Elite;

	//using Type = List;
	//auto& meshes{ scene.objects.Get<Object<Type>>() };

	//for (auto& vertex : meshes[1].vertices) // correcting tuktuk normals
	//	vertex.normal = { -vertex.normal.x, -vertex.normal.y, vertex.normal.z };

	//JL::Transform(meshes[1].vertices, JL::MakeScale<3>(3.f));
	//JL::Translate(meshes[1].vertices, { +30,  0, 0 });

	//JL::Transform(meshes[0].vertices, Elite::MakeRotationY(float(M_PI / 2)));
	//JL::Translate(meshes[0].vertices, { -30, +5, 0 });

}