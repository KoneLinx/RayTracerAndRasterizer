// CameraMovement.h - Camera movement implementations.

/* Copyright (C) 2020 Kobe Vrijsen

   this file is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 3.0 of the License, or (at your option) any later version.

   This file is made available in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; if not, see
   <https://www.gnu.org/licenses/>.

   Information in regards to this file:
   Contact:   kobevrijsen@posteo.be
*/

#pragma once

#include <set>
#include <SDL.h>
#include "MathExtra.h"
#include "JLCamera.h"
#include "JLVisitor.h"

namespace JL
{

	struct CameraMovement
	{
	
		// modify these values to match your preferences
		constexpr static bool  isAzerty            = false; // SDL will see azerty as querty. So don't change
		constexpr static bool  isEwsd              = true;
		constexpr static bool  isNumpad            = true;
		constexpr static bool  invertKeys          = false;
		constexpr static bool  invertMouseRotation = false;
		constexpr static bool  invertMouseMovement = false;
		constexpr static float movementFactor      = 2.f;
		constexpr static float distanceFactor      = 3.f;
		constexpr static float zoomFactor          = float(M_PI) / 5.f;
		constexpr static float mouseRotationFactor = 1.f / 200.f;
		constexpr static float mouseMovementFactor = 1.f / 100.f;
	
	private:
		
		struct KeyI
		{
			enum : size_t
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
				//froward         left            backward        right           up                  down
				{ SDL_SCANCODE_W, SDL_SCANCODE_A, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_SPACE, SDL_SCANCODE_LSHIFT }, // WASD
				{ SDL_SCANCODE_E, SDL_SCANCODE_W, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_SPACE, SDL_SCANCODE_LALT   }  // EWSD
			},
			// AZERTY
			{
				{ SDL_SCANCODE_Z, SDL_SCANCODE_Q, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_SPACE, SDL_SCANCODE_LSHIFT }, // ZQSD
				{ SDL_SCANCODE_E, SDL_SCANCODE_Z, SDL_SCANCODE_S, SDL_SCANCODE_D, SDL_SCANCODE_SPACE, SDL_SCANCODE_LALT   }  // EZSD
			}
		};
		constexpr static auto& _Key = _Keys[ isAzerty ][ isEwsd ];
	
		constexpr static SDL_Scancode _Arrows[2 /*is_numpad*/][KeyI::SIZE /*_key_i*/]
		{
			//froward          left               backward           right               up                   down
			{ SDL_SCANCODE_UP, SDL_SCANCODE_LEFT, SDL_SCANCODE_DOWN, SDL_SCANCODE_RIGHT, SDL_SCANCODE_PAGEUP, SDL_SCANCODE_PAGEDOWN }, // Arrows
			{ /*not used*/ }
		};
		constexpr static auto& _Arrow = _Arrows[ isNumpad ];
	
	public:
	
		struct Keyboard
		{
			using Type = std::decay_t<decltype(*SDL_GetKeyboardState(nullptr))>;
	
			constexpr static Type forward        = _Key  [ KeyI::forward  ];
			constexpr static Type left           = _Key  [ KeyI::left     ];
			constexpr static Type backward       = _Key  [ KeyI::backward ];
			constexpr static Type right          = _Key  [ KeyI::right    ];
			constexpr static Type up             = _Key  [ KeyI::up       ];
			constexpr static Type down           = _Key  [ KeyI::down     ];
	
			constexpr static Type arrow_forward  = _Arrow[ KeyI::forward  ];
			constexpr static Type arrow_left     = _Arrow[ KeyI::left     ];
			constexpr static Type arrow_backward = _Arrow[ KeyI::backward ];
			constexpr static Type arrow_right    = _Arrow[ KeyI::right    ];
			constexpr static Type arrow_up       = _Arrow[ KeyI::up       ];
			constexpr static Type arrow_down     = _Arrow[ KeyI::down     ];
	
			constexpr static Type fov_out        = SDL_SCANCODE_KP_MINUS;
			constexpr static Type fov_zoom       = SDL_SCANCODE_KP_PLUS ;
		};
	
		struct Mouse
		{
			using Type = std::decay_t<decltype(SDL_GetMouseState(nullptr, nullptr))>;
			
			constexpr static Type left           = SDL_BUTTON_LMASK;
			constexpr static Type middle         = SDL_BUTTON_MMASK;
			constexpr static Type right          = SDL_BUTTON_RMASK;
										         
			constexpr static Type walk           = left            ;
			constexpr static Type pivot          = right           ;
			constexpr static Type climb          = left | right    ;
		};
	
		static auto MakePollFunction()
		{
			return Visitor{

				// Key state
				[state = SDL_GetKeyboardState(nullptr)]
				(Keyboard::Type key) -> bool
				{
					return state[key] != 0;
				},

				// Mose state
				[state = SDL_GetMouseState(nullptr, nullptr)]
				(Mouse::Type key) -> bool
				{
					return state == key;
				}

			};
		}
	
		template <int N, typename T>
		static void Update(Camera<N, T>& camera, float const deltaT, float const deltaX = 0, float const deltaY = 0, float const deltaWheel = 0, bool freecam = true, typename Camera<N, T>::Point const& pivot = { 0, 0, 0 })
		{
			using Camera = Camera<N, T>;
	
			auto const isPressed = MakePollFunction();
	
			// Keyboard
			{
				
				float const factor = deltaT * (invertKeys ? -1 : 1) * movementFactor * (Distance(pivot, camera.GetPosition()) / distanceFactor);
	
				auto const unit{ MakeRotationUp(camera.GetDirection(), Camera::ROOT[1]) };
				typename Camera::Vector const& forward{ unit[0] };
				typename Camera::Vector const& up     { unit[1] };
				typename Camera::Vector const& left   { unit[2] };
	
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
					camera.SetFieldOfView(camera.GetFieldOfView() - zoomFactor * deltaT);
				else
				if (isPressed(Keyboard::fov_out))
					camera.SetFieldOfView(camera.GetFieldOfView() + zoomFactor * deltaT);
	
			}
	
			// Mouse
			{
	
				float const movementFactor = (invertMouseMovement ? -1 : 1) * mouseMovementFactor * (Distance(pivot, camera.GetPosition()) / distanceFactor);
				float const rotationFactor = (invertMouseRotation ? -1 : 1) * mouseRotationFactor;
	
				typename Camera::Vector const forward{ camera.GetDirection() };
				
				if (freecam)
				{
	
					typename Camera::Vector const& up{ Camera::ROOT[1] };
					typename Camera::Vector const left{ Cross(forward, up) };
	
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
	
				}
				else
				{
	
					auto const unit{ MakeRotationUp(forward, Camera::ROOT[1]) };
					typename Camera::Vector const& forward{ unit[0] };
					typename Camera::Vector const& up     { unit[1] };
					typename Camera::Vector const& left   { unit[2] };
	
					if (isPressed(Mouse::walk))
					{
						camera.SetPosition(camera.GetPosition() + movementFactor * deltaY * forward);
					}
					else
					if (isPressed(Mouse::pivot))
					{
						camera.TransformCamera(MakeRotation(up, deltaX * rotationFactor), pivot);
						camera.TransformCamera(MakeRotation(left, deltaY * rotationFactor), pivot);
					}
	
				}
	
				if (deltaWheel != 0.f)
				{
					float const factor = pow(1.01f, deltaWheel);
					float const focal = camera.GetFocalLength();
					auto const pos = camera.GetPosition();
	
					camera.SetFocalLength(focal * factor);
	
					if (freecam)
					{
						float distance = 10 * focal; // Should be the distance to the object in focus. for now, 10 seems fine.
						auto const dir = camera.GetDirection();
						camera.SetPosition(pos - (dir) * (distance * (factor - 1)));
					}
					else
					{
						camera.SetPosition(pos - (pivot - pos) * (factor - 1));
					}
	
				}
				
			}
	
			if (!freecam)
				camera.SetDirection(pivot - camera.GetPosition());
	
			return;
		}
	
	};


}