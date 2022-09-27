// Textures.h - provides texture mangement

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
#include "JLBaseIncludes.h"
#include <memory>
#include <unordered_map>
#include <SDL_image.h>

namespace JL
{

	struct Surface
	{
		SDL_Surface*				pSDLSurface;
		ID3D11Texture2D*			pDXTexture2DResource;
		ID3D11ShaderResourceView*	pDXResourceView;
	};

	class SurfaceManager 
	{

	public:

		using Surface = Surface;
		using SurfaceID = size_t;
		using SurfaceMap = std::unordered_map<SurfaceID, Surface, IdentityFunctor>;
		using SurfacePair = Aggregate<Wrapper<SurfaceID>, std::string_view>;

		SurfaceManager() noexcept;
		~SurfaceManager() noexcept;

		SurfaceManager(ID3D11Device* pDevice, std::initializer_list<SurfacePair> surfaces);

		Surface& operator [] (SurfaceMap::key_type const id);
		Surface  operator [] (SurfaceMap::key_type const id) const;

		static SurfacePair GenID(std::string_view file) noexcept;

	private:

		SurfaceMap m_SurfaceMap;
		
		Surface Load(ID3D11Device* pDevice, std::string_view file) const;

		SurfaceManager(SurfaceManager &&) noexcept = default;
		SurfaceManager& operator = (SurfaceManager&&) noexcept = default;

		// Avoid copy
		SurfaceManager(SurfaceManager const&) = delete;
		SurfaceManager& operator = (SurfaceManager const&) = delete;


	};

}