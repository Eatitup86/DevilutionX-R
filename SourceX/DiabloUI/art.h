#pragma once

#include "devilution.h"

namespace dvl {

struct Art {
	SDL_Surface *surface = NULL;
	int frames = 1;
	int frame_height = 0;
	unsigned int palette_version = 0;

	int w() const
	{
		return surface->w;
	}

	int h() const
	{
		return frame_height;
	}

	void Unload()
	{
		SDL_FreeSurface(surface);
		surface = NULL;
	}
};

void LoadArt(char *pszFile, Art *art, int frames = 1, PALETTEENTRY *pPalette = NULL);

}
