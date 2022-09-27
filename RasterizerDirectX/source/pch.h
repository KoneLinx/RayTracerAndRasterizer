#pragma once

constexpr bool DEBUGMODE =
#if (defined(_DEBUG) || defined(DEBUG))
	true
	#if !defined(DEBUG)
		#define DEBUG
	#endif
#else
	false
#endif
;



#define SHADER shaders::rasterizer



#include <iostream>
#include <algorithm>
#include <array>
#include <vector>
#include <memory>
#define NOMINMAX  //for directx

// SDL Headers
#include "SDL.h"
#include "SDL_syswm.h"
#include "SDL_surface.h"

// DirectX Headers
#include <dxgi.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <d3dx11effect.h>

//Elite headers
#include "Elite/EMath.h"
#include "Elite/ERGBColor.h"
using namespace Elite;


using IDK         =  char;

using  StrLiteral =  char   const [];
using WStrLiteral = wchar_t const [];

using  StrRaw     =  char   const * const;
using WStrRaw     = wchar_t const * const;