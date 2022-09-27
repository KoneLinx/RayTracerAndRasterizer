#pragma once

#include <set>
#include <SDL.h>
#include "JL/JLMathUtilities.h"
#include "JL/JLCamera.h"
#include "JL/JLVisitor.h"

struct CameraMovement
{

	// modify these values to match your preferences
	constexpr static bool isAzerty = false;
	constexpr static bool isEwsd   = true;
	constexpr static bool invertKeys  = false;
	constexpr static bool invertMouseRotation = false;
	constexpr static bool invertMouseMovement = false;
	constexpr static float movementFactor = 2.f;
	constexpr static float mouseRotationFactor = 1.f / 200.f;
	constexpr static float mouseMovementFactor = 1.f / 100.f;

private:
	
	struct KeyI
	{
		enum
		{
			forward = 0,
			left,
			backward,
			right,
			up,
			down,

			SIZE
		};
	};

	constexpr static SDL_Scancode _Keys[2 /*is_azerty*/][2 /*is_ewsd*/][KeyI::SIZE /*_key_i*/]
	{
		// QWERTY
		{
			{ SDL_SCANCODE_W, SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_SPACE, SDL_SCANCODE_LSHIFT }, // WASD
			{ SDL_SCANCODE_E, SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_SPACE, SDL_SCANCODE_LALT   }  // EWSD
		},
		// AZERTY
		{
			{ SDL_SCANCODE_Z, SDL_SCANCODE_Q, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_SPACE, SDL_SCANCODE_LSHIFT }, // ZQSD
			{ SDL_SCANCODE_E, SDL_SCANCODE_Z, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_SPACE, SDL_SCANCODE_LALT   }  // EZSD
		}
	};

public:

	struct Keyboard
	{
		using Type =
		enum : std::decay_t<decltype(*SDL_GetKeyboardState(nullptr))>
		{							   
			forward  = _Keys[ isAzerty ][ isEwsd ][ KeyI::forward  ],
			left     = _Keys[ isAzerty ][ isEwsd ][ KeyI::left     ],
			backward = _Keys[ isAzerty ][ isEwsd ][ KeyI::backward ],
			right    = _Keys[ isAzerty ][ isEwsd ][ KeyI::right    ],
			up       = _Keys[ isAzerty ][ isEwsd ][ KeyI::up       ],
			down     = _Keys[ isAzerty ][ isEwsd ][ KeyI::down     ],
			arrow_forward  = SDL_SCANCODE_UP,
			arrow_left     = SDL_SCANCODE_LEFT,
			arrow_backward = SDL_SCANCODE_DOWN,
			arrow_right    = SDL_SCANCODE_RIGHT,
			arrow_up       = SDL_SCANCODE_KP_0,
			arrow_down     = SDL_SCANCODE_RSHIFT,
			fov_out  = SDL_SCANCODE_KP_MINUS,
			fov_zoom = SDL_SCANCODE_KP_PLUS,
		};
	};

	struct Mouse
	{
		using Type =
		enum : std::decay_t<decltype(SDL_GetMouseState(nullptr, nullptr))>
		{
			left = SDL_BUTTON_LMASK,
			middle = SDL_BUTTON_MMASK,
			right = SDL_BUTTON_RMASK,

			walk  = left,
			pivot = right,
			climb = left | right,
		};
	};

	template<int N, typename T>
	static void Update(JL::Camera<N, T> & camera, float const deltaT, float const deltaX = 0, float const deltaY = 0, float const deltaWheel = 0)
	{
		using Camera = JL::Camera<N, T>;
		using namespace JL;

		auto const isPressed = MakePollFunction();
	
		// Keyboard
		{
			auto const unit{ MakeRotation(camera.GetDirection(), Camera::ROOT[1]) };
			typename Camera::Vector const& forward{ unit[0] };
			typename Camera::Vector const& up     { unit[1] };
			typename Camera::Vector const& left   { unit[2] };
			
			float const factor = deltaT * (invertKeys ? -1 : 1) * movementFactor;

			if (isPressed(Keyboard::forward) || isPressed(Keyboard::arrow_forward))
				camera.SetPosition(camera.GetPosition() + factor * forward);
			else
			if (isPressed(Keyboard::backward) || isPressed(Keyboard::arrow_backward))
				camera.SetPosition(camera.GetPosition() - factor * forward);
	
			if (isPressed(Keyboard::left) || isPressed(Keyboard::arrow_left))
				camera.SetPosition(camera.GetPosition() + factor * left);
			else
			if (isPressed(Keyboard::right) || isPressed(Keyboard::arrow_right))
				camera.SetPosition(camera.GetPosition() - factor * left);

			if (isPressed(Keyboard::up) || isPressed(Keyboard::arrow_up))
				camera.SetPosition(camera.GetPosition() + factor * up);
			else
			if (isPressed(Keyboard::down) || isPressed(Keyboard::arrow_down))
				camera.SetPosition(camera.GetPosition() - factor * up);

			if (isPressed(Keyboard::fov_zoom))
				camera.SetFieldOfView(camera.GetFieldOfView() - float(M_PI) / 25.f);
			else
			if(isPressed(Keyboard::fov_out))
				camera.SetFieldOfView(camera.GetFieldOfView() + float(M_PI) / 25.f);
		}

		// Mouse
		{
			typename Camera::Vector const& up{ Camera::ROOT[1] };
			typename Camera::Vector const forward{ /*ProjectNormal(*/camera.GetDirection()/*, up)*/ };
			typename Camera::Vector const left{ Cross(forward, up) };

			float const movementFactor = (invertMouseMovement ? -1 : 1) * mouseMovementFactor;
			float const rotationFactor = (invertMouseRotation ? -1 : 1) * mouseRotationFactor;

			if (isPressed(Mouse::climb))
			{
				camera.SetPosition(camera.GetPosition() + movementFactor * deltaY * up);
			}
			else
			if (isPressed(Mouse::walk))
			{
				camera.SetPosition(camera.GetPosition() + movementFactor * deltaY * forward);
				camera.TransformCamera(MakeRotation(up, deltaX * rotationFactor));
			}
			else
			if (isPressed(Mouse::pivot))
			{
				camera.TransformCamera(MakeRotation(up, deltaX * rotationFactor));
				camera.TransformCamera(MakeRotation(left, deltaY * rotationFactor));
			}

			if (deltaWheel != 0.f)
				camera.SetFocalLength(camera.GetFocalLength() * pow(1.01, deltaWheel));
		}

		return;
	}

private:

	static auto MakePollFunction()
	{
		return JL::Visitor{
			[state = SDL_GetKeyboardState(nullptr)]
			(Keyboard::Type key) -> bool
			{
				return state[key];
			},
			[state = SDL_GetMouseState(nullptr, nullptr)]
			(Mouse::Type key) -> bool
			{
				return (state ^ key) == 0;
			}
		};
	}

};
