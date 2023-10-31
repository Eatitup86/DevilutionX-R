#pragma once

#include <cstdint>

#include "utils/enum_traits.h"

namespace devilution {

enum class UiFlags : uint32_t {
	// clang-format off
	None               = 0,

	FontSize12         = 1 << 0,
	FontSize24         = 1 << 1,
	FontSize30         = 1 << 2,
	FontSize42         = 1 << 3,
	FontSize46         = 1 << 4,
	FontSizeDialog     = 1 << 5,

	ColorUiGold        = 1 << 6,
	ColorUiSilver      = 1 << 7,
	ColorUiGoldDark    = 1 << 8,
	ColorUiSilverDark  = 1 << 9,
	ColorDialogWhite   = 1 << 10,
	ColorDialogRed     = 1 << 11,
	ColorYellow        = 1 << 12,
	ColorGold          = 1 << 13,
	ColorBlack         = 1 << 14,
	ColorWhite         = 1 << 15,
	ColorWhitegold     = 1 << 16,
	ColorRed           = 1 << 17,
	ColorBlue          = 1 << 18,
	ColorOrange        = 1 << 19,
	ColorButtonface    = 1 << 20,
	ColorButtonpushed  = 1 << 21,

	AlignCenter        = 1 << 22,
	AlignRight         = 1 << 23,
	VerticalCenter     = 1 << 24,

	KerningFitSpacing  = 1 << 25,

	ElementDisabled    = 1 << 26,
	ElementHidden      = 1 << 27,

	PentaCursor        = 1 << 28,
	Outlined           = 1 << 29,

	/** @brief Ensures that the if current element is active that the next element is also visible. */
	NeedsNextElement   = 1 << 30,
	// clang-format on
};
use_enum_as_flags(UiFlags);

} // namespace devilution
