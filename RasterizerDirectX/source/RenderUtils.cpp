#include "pch.h"

#include "RenderUtils.h"

Elite::NDCPoint Elite::RasterToNCD(const RasterPoint value, const RasterValue width, const RasterValue height)
{
	return NDCPoint{
		static_cast<NDCValue>(2) * (static_cast<NDCValue>(value.x) + static_cast<NDCValue>(0.5)) / static_cast<NDCValue>(width ) - static_cast<NDCValue>(1),
		static_cast<NDCValue>(1) - (static_cast<NDCValue>(value.y) + static_cast<NDCValue>(0.5)) / static_cast<NDCValue>(height) * static_cast<NDCValue>(2)
	};
}

Elite::RasterPoint Elite::NDCToRaster(const NDCPoint value, const RasterValue width, const RasterValue height)
{
	return RasterPoint{
		static_cast<RasterValue>(static_cast<NDCValue>(width ) / static_cast<NDCValue>(2) * (static_cast<NDCValue>(1) + value.x) - static_cast<NDCValue>(0.5)),
		static_cast<RasterValue>(static_cast<NDCValue>(height) / static_cast<NDCValue>(2) * (static_cast<NDCValue>(1) - value.y) - static_cast<NDCValue>(0.5))
	};
}

Elite::Projection& Elite::PointToRaster(Projection& value, const RasterValue width, const RasterValue height)
{
	// Analog to NDCToRaster()
	value.x	= static_cast<WorldValue>(static_cast<WorldValue>(width ) / static_cast<WorldValue>(2) * (static_cast<WorldValue>(1) + value.x) - static_cast<WorldValue>(0.5));
	value.y = static_cast<WorldValue>(static_cast<WorldValue>(height) / static_cast<WorldValue>(2) * (static_cast<WorldValue>(1) - value.y) - static_cast<WorldValue>(0.5));
	return value;
}

Elite::PixelValue* Elite::GetPixels(SDL_Surface* pSurface)
{
	return static_cast<PixelValue*>(pSurface->pixels);
}