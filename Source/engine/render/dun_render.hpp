/**
 * @file dun_render.hpp
 *
 * Interface of functionality for rendering the level tiles.
 */
#pragma once

#include <cstdint>

#include <SDL_endian.h>

#include "engine.h"

namespace devilution {

/**
 * Level tile type.
 *
 * The tile type determines data encoding and the shape.
 *
 * Each tile type has its own encoding but they all encode data in the order
 * of bottom-to-top (bottom row first).
 */
enum class TileType : uint8_t {
	/**
	 * 🮆 A 32x32 square. Stored as an array of pixels.
	 */
	Square,

	/**
	 * 🮆 A 32x32 square with transparency. RLE encoded.
	 *
	 * Each run starts with an int8_t value.
	 * If positive, it is followed by this many pixels.
	 * If negative, it indicates `-value` fully transparent pixels, which are omitted.
	 *
	 * Runs do not cross row boundaries.
	 */
	TransparentSquare,

	/**
	 *🭮 Left-pointing 32x31 triangle. Encoded as 31 varying-width rows with 2 padding bytes before every even row.
	 *
	 * The smallest rows (bottom and top) are 2px wide, the largest row is 16px wide (middle row).
	 *
	 * Encoding:
	 * for i in [0, 30]:
	 * - 2 unused bytes if i is even
	 * - row (only the pixels within the triangle)
	 */
	LeftTriangle,

	/**
	 * 🭬Right-pointing 32x31 triangle.  Encoded as 31 varying-width rows with 2 padding bytes after every even row.
	 *
	 * The smallest rows (bottom and top) are 2px wide, the largest row is 16px wide (middle row).
	 *
	 * Encoding:
	 * for i in [0, 30]:
	 * - row (only the pixels within the triangle)
	 * - 2 unused bytes if i is even
	 */
	RightTriangle,

	/**
	 * 🭓 Left-pointing 32x32 trapezoid: a 32x16 rectangle and the 16x16 bottom part of `LeftTriangle`.
	 *
	 * Begins with triangle part, which uses the `LeftTriangle` encoding,
	 * and is followed by a flat array of pixels for the top rectangle part.
	 */
	LeftTrapezoid,

	/**
	 * 🭞 Right-pointing 32x32 trapezoid: 32x16 rectangle and the 16x16 bottom part of `RightTriangle`.
	 *
	 * Begins with the triangle part, which uses the `RightTriangle` encoding,
	 * and is followed by a flat array of pixels for the top rectangle part.
	 */
	RightTrapezoid,
};

/**
 * Specifies the type of arches to render.
 */
enum class ArchType {
	None,
	Left,
	Right
};

/**
 * Specifies the current MIN block of the level CEL file, as used during rendering of the level tiles.
 */
class LevelCelBlock {
public:
	explicit LevelCelBlock(uint16_t data)
	    : data_(data)
	{
	}

	[[nodiscard]] bool hasValue() const
	{
		return data_ != 0;
	}

	[[nodiscard]] TileType type() const
	{
		return static_cast<TileType>((data_ & 0x7000) >> 12);
	}

	[[nodiscard]] uint16_t frame() const
	{
		return SDL_SwapLE32(data_ & 0xFFF);
	}

private:
	uint16_t data_;
};

/**
 * @brief Blit current world CEL to the given buffer
 * @param out Target buffer
 * @param position Target buffer coordinates
 * @param levelCelBlock The MIN block of the level CEL file.
 * @param levelPieceId The piece ID (index into SOLData and DPieceMicros).
 * @param archType The type of arch to render.
 */
void RenderTile(const Surface &out, Point position, LevelCelBlock levelCelBlock, uint16_t levelPieceId, ArchType archType);

/**
 * @brief Render a black 64x31 tile ◆
 * @param out Target buffer
 * @param sx Target buffer coordinate (left corner of the tile)
 * @param sy Target buffer coordinate (bottom corner of the tile)
 */
void world_draw_black_tile(const Surface &out, int sx, int sy);

} // namespace devilution
