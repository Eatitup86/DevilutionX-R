#include "art.h"

#include <cstddef>
#include <cstdint>
#include <memory>

#include "DiabloUI/art.h"
#include "storm/storm.h"
#include "utils/display.h"
#include "utils/log.hpp"
#include "utils/sdl_compat.h"

namespace devilution {
namespace {
constexpr unsigned PcxHeaderSize = 128;
constexpr unsigned NumPaletteColors = 256;
constexpr unsigned PcxPaletteSize = 1 + NumPaletteColors * 3;

bool LoadPcxMeta(HANDLE handle, int &width, int &height, std::uint8_t &bpp)
{
	PCXHeader pcxhdr;
	if (!SFileReadFile(handle, &pcxhdr, PcxHeaderSize, nullptr, nullptr)) {
		return false;
	}
	width = SDL_SwapLE16(pcxhdr.Xmax) - SDL_SwapLE16(pcxhdr.Xmin) + 1;
	height = SDL_SwapLE16(pcxhdr.Ymax) - SDL_SwapLE16(pcxhdr.Ymin) + 1;
	bpp = pcxhdr.BitsPerPixel;
	return true;
}

bool LoadPcxPixelsAndPalette(HANDLE handle, int width, int height, std::uint8_t bpp,
    BYTE *buffer, std::size_t bufferPitch, SDL_Color *palette)
{
	const bool has256ColorPalette = palette != nullptr && bpp == 8;
	std::uint32_t pixelDataSize = SFileGetFileSize(handle, nullptr);
	if (pixelDataSize == static_cast<std::uint32_t>(-1)) {
		return false;
	}
	pixelDataSize -= PcxHeaderSize + (has256ColorPalette ? PcxPaletteSize : 0);

	// We read 1 extra byte because it delimits the palette.
	const unsigned readSize = pixelDataSize + (has256ColorPalette ? PcxPaletteSize : 0);
	std::unique_ptr<BYTE[]> fileBuffer = std::make_unique<BYTE[]>(readSize);
	if (!SFileReadFile(handle, fileBuffer.get(), readSize, nullptr, nullptr)) {
		return false;
	}
	const unsigned xSkip = bufferPitch - width;
	BYTE *dataPtr = fileBuffer.get();
	for (int j = 0; j < height; j++) {
		for (int x = 0; x < width;) {
			constexpr std::uint8_t PcxMaxSinglePixel = 0xBF;
			const std::uint8_t byte = *dataPtr++;
			if (byte <= PcxMaxSinglePixel) {
				*buffer++ = byte;
				++x;
				continue;
			}
			constexpr std::uint8_t PcxRunLengthMask = 0x3F;
			const std::uint8_t runLength = (byte & PcxRunLengthMask);
			std::memset(buffer, *dataPtr++, runLength);
			buffer += runLength;
			x += runLength;
		}
		buffer += xSkip;
	}

	if (has256ColorPalette) {
		[[maybe_unused]] constexpr unsigned PcxPaletteSeparator = 0x0C;
		assert(*dataPtr == PcxPaletteSeparator);
		++dataPtr;

		auto *out = palette;
		for (unsigned i = 0; i < NumPaletteColors; ++i) {
			out->r = *dataPtr++;
			out->g = *dataPtr++;
			out->b = *dataPtr++;
#ifndef USE_SDL1
			out->a = SDL_ALPHA_OPAQUE;
#endif
			++out;
		}
	}
	return true;
}

Uint32 GetPcxSdlPixelFormat(unsigned bpp)
{
	switch (bpp) {
	case 8: // NOLINT(readability-magic-numbers)
		return SDL_PIXELFORMAT_INDEX8;
	case 24: // NOLINT(readability-magic-numbers)
		return SDL_PIXELFORMAT_RGB888;
	case 32: // NOLINT(readability-magic-numbers)
		return SDL_PIXELFORMAT_RGBA8888;
	default:
		return 0;
	}
}

} // namespace

void LoadArt(const char *pszFile, Art *art, int frames, SDL_Color *pPalette)
{
	if (art == nullptr || art->surface != nullptr)
		return;

	art->frames = frames;

	HANDLE handle;
	int width;
	int height;
	std::uint8_t bpp;
	if (!SFileOpenFile(pszFile, &handle)) {
		return;
	}

	if (!LoadPcxMeta(handle, width, height, bpp)) {
		Log("LoadArt(\"{}\"): LoadPcxMeta failed with code {}", pszFile, SErrGetLastError());
		SFileCloseFile(handle);
		return;
	}

	SDLSurfaceUniquePtr artSurface { SDL_CreateRGBSurfaceWithFormat(SDL_SWSURFACE, width, height, bpp, GetPcxSdlPixelFormat(bpp)) };
	if (!LoadPcxPixelsAndPalette(handle, width, height, bpp, static_cast<BYTE *>(artSurface->pixels),
	        artSurface->pitch, pPalette)) {
		Log("LoadArt(\"{}\"): LoadPcxPixelsAndPalette failed with code {}", pszFile, SErrGetLastError());
		SFileCloseFile(handle);
		return;
	}
	SFileCloseFile(handle);

	art->logical_width = artSurface->w;
	art->frame_height = height / frames;

	art->surface = ScaleSurfaceToOutput(std::move(artSurface));
}

void LoadMaskedArt(const char *pszFile, Art *art, int frames, int mask)
{
	LoadArt(pszFile, art, frames);
	if (art->surface != nullptr)
		SDLC_SetColorKey(art->surface.get(), mask);
}

void LoadArt(Art *art, const BYTE *artData, int w, int h, int frames)
{
	constexpr int DefaultArtBpp = 8;
	constexpr int DefaultArtFormat = SDL_PIXELFORMAT_INDEX8;
	art->frames = frames;
	art->surface = ScaleSurfaceToOutput(SDLSurfaceUniquePtr { SDL_CreateRGBSurfaceWithFormatFrom(
	    const_cast<BYTE *>(artData), w, h, DefaultArtBpp, w, DefaultArtFormat) });
	art->logical_width = w;
	art->frame_height = h / frames;
}

} // namespace devilution
