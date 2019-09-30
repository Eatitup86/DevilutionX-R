#pragma once

#include <cstdint>
#include <string>
#include <vector>

#include "devilution.h"
#include "art.h"
#include "stubs.h"

namespace dvl {

enum UiType {
	UI_TEXT,
	UI_IMAGE,
	UI_BUTTON,
	UI_LIST,
	UI_EDIT,
};

enum UiFlags {
	UIS_SMALL = 1 << 0,
	UIS_MED = 1 << 1,
	UIS_BIG = 1 << 2,
	UIS_HUGE = 1 << 3,
	UIS_CENTER = 1 << 4,
	UIS_RIGHT = 1 << 5,
	UIS_VCENTER = 1 << 6,
	UIS_SILVER = 1 << 7,
	UIS_GOLD = 1 << 8,
	UIS_SML1 = 1 << 9,
	UIS_SML2 = 1 << 10,
	UIS_LIST = 1 << 11,
	UIS_DISABLED = 1 << 12,
	UIS_HIDDEN = 1 << 13,
};

struct UiItemBase {
	constexpr UiItemBase(SDL_Rect rect, int flags)
	    : rect(rect)
	    , flags(flags)
	{
	}
	SDL_Rect rect;
	int flags;
};

struct UiImage : public UiItemBase {
	constexpr UiImage(Art *art, SDL_Rect rect, int flags = 0)
	    : UiImage(art, /*frame=*/0, rect, flags)
	{
	}

	constexpr UiImage(Art *art, int frame, SDL_Rect rect, int flags = 0)
	    : UiItemBase(rect, flags)
	    , art(art)
	    , frame(frame)
	{
	}

	Art *art;
	int frame;
};

struct UiText : public UiItemBase {
	constexpr UiText(const char *text, SDL_Rect rect, int flags = 0)
	    : UiItemBase(rect, flags)
	    , text(text)
	{
	}

	const char *text;
};

struct UiButton : public UiItemBase {
	constexpr UiButton(const char *text, void (*action)(), SDL_Rect rect, int flags = 0)
	    : UiItemBase(rect, flags)
	    , text(text)
	    , action(action)
	{
	}

	const char *text;
	void (*action)();
};

struct UiListItem {
	constexpr UiListItem(const char *text = "", int value = 0)
	    : text(text)
	    , value(value)
	{
	}
	const char *text;
	int value;
};

struct UiList : public UiItemBase {
	template <std::size_t N>
	constexpr UiList(
	    UiListItem (&items)[N],
	    decltype(SDL_Rect().x) x,
	    decltype(SDL_Rect().y) y,
	    decltype(SDL_Rect().w) item_width,
	    decltype(SDL_Rect().h) item_height,
	    int flags)
	    : UiItemBase({ x, y, item_width, static_cast<decltype(SDL_Rect().h)>(item_height * N) }, flags)
	    , x(x)
	    , y(y)
	    , item_width(item_width)
	    , item_height(item_height)
	    , items(items)
	    , length(N)
	{
	}

	decltype(SDL_Rect().x) x;
	decltype(SDL_Rect().y) y;
	decltype(SDL_Rect().w) item_width;
	decltype(SDL_Rect().h) item_height;
	UiListItem *items;
	std::size_t length;

	SDL_Rect itemRect(std::size_t i) const
	{
		return { x, static_cast<decltype(SDL_Rect().y)>(y + item_height * i), item_width, item_height };
	}

	UiListItem *itemAt(decltype(SDL_Rect().y) y) const
	{
		ASSERT(y >= rect.y);
		const std::size_t index = (y - rect.y) / item_height;
		ASSERT(index < length);
		return &items[index];
	}
};

struct UiEdit : public UiItemBase {
	constexpr UiEdit(char *value, std::size_t max_length, SDL_Rect rect, int flags)
	    : UiItemBase(rect, flags)
	    , value(value)
	    , max_length(max_length)

	{
	}

	char *value;
	std::size_t max_length;
};

struct UiItem {
	constexpr UiItem(UiText text)
	    : type(UI_TEXT)
	    , text(text)
	{
	}

	constexpr UiItem(UiImage image)
	    : type(UI_IMAGE)
	    , image(image)
	{
	}

	constexpr UiItem(UiButton button)
	    : type(UI_BUTTON)
	    , button(button)
	{
	}

	constexpr UiItem(UiList list)
	    : type(UI_LIST)
	    , list(list)
	{
	}

	constexpr UiItem(UiEdit edit)
	    : type(UI_EDIT)
	    , edit(edit)
	{
	}

	UiType type;
	union {
		UiText text;
		UiImage image;
		UiButton button;
		UiList list;
		UiEdit edit;
		UiItemBase common;
	};

	int flags() const
	{
		return common.flags;
	}

	const SDL_Rect &rect() const
	{
		return common.rect;
	}
};

} // namespace dvl
