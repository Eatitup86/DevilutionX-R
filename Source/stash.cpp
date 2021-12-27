/**
 * @file stash.cpp
 *
 * Implementation of player stash.
 */
#include <utility>

#include <algorithm>
#include <fmt/format.h>

#include "controls/plrctrls.h"
#include "cursor.h"
#include "engine/cel_sprite.hpp"
#include "engine/load_cel.hpp"
#include "engine/rectangle.hpp"
#include "engine/render/cel_render.hpp"
#include "engine/render/text_render.hpp"
#include "engine/size.hpp"
#include "hwcursor.hpp"
#include "inv_iterators.hpp"
#include "minitext.h"
#include "options.h"
#include "pfile.h"
#include "plrmsg.h"
#include "stash.h"
#include "stores.h"
#include "town.h"
#include "towners.h"
#include "utils/language.h"
#include "utils/paths.h"
#include "utils/sdl_geometry.h"
#include "utils/stdcompat/optional.hpp"
#include <fstream>
#include <iostream>

namespace devilution {

bool stashflag;
int stashGold;

std::streampos size;

StashStruct Stash;

int Page; // current page in the stash

int stashX = 17;                          // initial stash cell x coordinate
int stashY = 76;                          // initial stash cell y coordinate
int stashNextCell = INV_SLOT_SIZE_PX + 1; // spacing between each cell

struct StyledText {
	UiFlags style;
	std::string text;
	int spacing = 1;
};

struct PanelEntry {
	std::string label;
	Point position;
	int length;
	int labelLength;                                       // max label's length - used for line wrapping
	std::function<StyledText()> statDisplayFunc = nullptr; // function responsible for displaying stat
};

PanelEntry stashEntries[] = {
	{ N_("Page"), { 11, 0 }, 0, 15 },
	{ "", { 11, 11 }, 16, 0,
	    []() { return StyledText { UiFlags::ColorWhite, fmt::format("{:d}", Page + 1) }; } },
	{ N_("Gold"), { 128, 0 }, 0, 98 },
	{ "", { 128, 11 }, 99, 0,
	    []() { return StyledText { UiFlags::ColorWhite, fmt::format("{:d}", stashGold) }; } },
};

const Point StashRect[] = {
	// Contains mappings for each cell in the stash (10x10)
	// clang-format off
	//  X,   Y
	// row 1
	{ stashX + stashNextCell * 0,  stashY + stashNextCell * 0 },
	{ stashX + stashNextCell * 1,  stashY + stashNextCell * 0 },
	{ stashX + stashNextCell * 2,  stashY + stashNextCell * 0 },
	{ stashX + stashNextCell * 3,  stashY + stashNextCell * 0 },
	{ stashX + stashNextCell * 4,  stashY + stashNextCell * 0 },
	{ stashX + stashNextCell * 5,  stashY + stashNextCell * 0 },
	{ stashX + stashNextCell * 6,  stashY + stashNextCell * 0 },
	{ stashX + stashNextCell * 7,  stashY + stashNextCell * 0 },
	{ stashX + stashNextCell * 8,  stashY + stashNextCell * 0 },
	{ stashX + stashNextCell * 9,  stashY + stashNextCell * 0 },
	// row 2
	{ stashX + stashNextCell * 0,  stashY + stashNextCell * 1 },
	{ stashX + stashNextCell * 1,  stashY + stashNextCell * 1 },
	{ stashX + stashNextCell * 2,  stashY + stashNextCell * 1 },
	{ stashX + stashNextCell * 3,  stashY + stashNextCell * 1 },
	{ stashX + stashNextCell * 4,  stashY + stashNextCell * 1 },
	{ stashX + stashNextCell * 5,  stashY + stashNextCell * 1 },
	{ stashX + stashNextCell * 6,  stashY + stashNextCell * 1 },
	{ stashX + stashNextCell * 7,  stashY + stashNextCell * 1 },
	{ stashX + stashNextCell * 8,  stashY + stashNextCell * 1 },
	{ stashX + stashNextCell * 9,  stashY + stashNextCell * 1 },
	// row 3
	{ stashX + stashNextCell * 0,  stashY + stashNextCell * 2 },
	{ stashX + stashNextCell * 1,  stashY + stashNextCell * 2 },
	{ stashX + stashNextCell * 2,  stashY + stashNextCell * 2 },
	{ stashX + stashNextCell * 3,  stashY + stashNextCell * 2 },
	{ stashX + stashNextCell * 4,  stashY + stashNextCell * 2 },
	{ stashX + stashNextCell * 5,  stashY + stashNextCell * 2 },
	{ stashX + stashNextCell * 6,  stashY + stashNextCell * 2 },
	{ stashX + stashNextCell * 7,  stashY + stashNextCell * 2 },
	{ stashX + stashNextCell * 8,  stashY + stashNextCell * 2 },
	{ stashX + stashNextCell * 9,  stashY + stashNextCell * 2 },
	// row 4
	{ stashX + stashNextCell * 0,  stashY + stashNextCell * 3 },
	{ stashX + stashNextCell * 1,  stashY + stashNextCell * 3 },
	{ stashX + stashNextCell * 2,  stashY + stashNextCell * 3 },
	{ stashX + stashNextCell * 3,  stashY + stashNextCell * 3 },
	{ stashX + stashNextCell * 4,  stashY + stashNextCell * 3 },
	{ stashX + stashNextCell * 5,  stashY + stashNextCell * 3 },
	{ stashX + stashNextCell * 6,  stashY + stashNextCell * 3 },
	{ stashX + stashNextCell * 7,  stashY + stashNextCell * 3 },
	{ stashX + stashNextCell * 8,  stashY + stashNextCell * 3 },
	{ stashX + stashNextCell * 9,  stashY + stashNextCell * 3 },
	// row 5
	{ stashX + stashNextCell * 0,  stashY + stashNextCell * 4 },
	{ stashX + stashNextCell * 1,  stashY + stashNextCell * 4 },
	{ stashX + stashNextCell * 2,  stashY + stashNextCell * 4 },
	{ stashX + stashNextCell * 3,  stashY + stashNextCell * 4 },
	{ stashX + stashNextCell * 4,  stashY + stashNextCell * 4 },
	{ stashX + stashNextCell * 5,  stashY + stashNextCell * 4 },
	{ stashX + stashNextCell * 6,  stashY + stashNextCell * 4 },
	{ stashX + stashNextCell * 7,  stashY + stashNextCell * 4 },
	{ stashX + stashNextCell * 8,  stashY + stashNextCell * 4 },
	{ stashX + stashNextCell * 9,  stashY + stashNextCell * 4 },
	// row 6
	{ stashX + stashNextCell * 0,  stashY + stashNextCell * 5 },
	{ stashX + stashNextCell * 1,  stashY + stashNextCell * 5 },
	{ stashX + stashNextCell * 2,  stashY + stashNextCell * 5 },
	{ stashX + stashNextCell * 3,  stashY + stashNextCell * 5 },
	{ stashX + stashNextCell * 4,  stashY + stashNextCell * 5 },
	{ stashX + stashNextCell * 5,  stashY + stashNextCell * 5 },
	{ stashX + stashNextCell * 6,  stashY + stashNextCell * 5 },
	{ stashX + stashNextCell * 7,  stashY + stashNextCell * 5 },
	{ stashX + stashNextCell * 8,  stashY + stashNextCell * 5 },
	{ stashX + stashNextCell * 9,  stashY + stashNextCell * 5 },
	// row 7
	{ stashX + stashNextCell * 0,  stashY + stashNextCell * 6 },
	{ stashX + stashNextCell * 1,  stashY + stashNextCell * 6 },
	{ stashX + stashNextCell * 2,  stashY + stashNextCell * 6 },
	{ stashX + stashNextCell * 3,  stashY + stashNextCell * 6 },
	{ stashX + stashNextCell * 4,  stashY + stashNextCell * 6 },
	{ stashX + stashNextCell * 5,  stashY + stashNextCell * 6 },
	{ stashX + stashNextCell * 6,  stashY + stashNextCell * 6 },
	{ stashX + stashNextCell * 7,  stashY + stashNextCell * 6 },
	{ stashX + stashNextCell * 8,  stashY + stashNextCell * 6 },
	{ stashX + stashNextCell * 9,  stashY + stashNextCell * 6 },
	// row 8
	{ stashX + stashNextCell * 0,  stashY + stashNextCell * 7 },
	{ stashX + stashNextCell * 1,  stashY + stashNextCell * 7 },
	{ stashX + stashNextCell * 2,  stashY + stashNextCell * 7 },
	{ stashX + stashNextCell * 3,  stashY + stashNextCell * 7 },
	{ stashX + stashNextCell * 4,  stashY + stashNextCell * 7 },
	{ stashX + stashNextCell * 5,  stashY + stashNextCell * 7 },
	{ stashX + stashNextCell * 6,  stashY + stashNextCell * 7 },
	{ stashX + stashNextCell * 7,  stashY + stashNextCell * 7 },
	{ stashX + stashNextCell * 8,  stashY + stashNextCell * 7 },
	{ stashX + stashNextCell * 9,  stashY + stashNextCell * 7 },
	// row 9
	{ stashX + stashNextCell * 0,  stashY + stashNextCell * 8 },
	{ stashX + stashNextCell * 1,  stashY + stashNextCell * 8 },
	{ stashX + stashNextCell * 2,  stashY + stashNextCell * 8 },
	{ stashX + stashNextCell * 3,  stashY + stashNextCell * 8 },
	{ stashX + stashNextCell * 4,  stashY + stashNextCell * 8 },
	{ stashX + stashNextCell * 5,  stashY + stashNextCell * 8 },
	{ stashX + stashNextCell * 6,  stashY + stashNextCell * 8 },
	{ stashX + stashNextCell * 7,  stashY + stashNextCell * 8 },
	{ stashX + stashNextCell * 8,  stashY + stashNextCell * 8 },
	{ stashX + stashNextCell * 9,  stashY + stashNextCell * 8 },
	// row 10
	{ stashX + stashNextCell * 0,  stashY + stashNextCell * 9 },
	{ stashX + stashNextCell * 1,  stashY + stashNextCell * 9 },
	{ stashX + stashNextCell * 2,  stashY + stashNextCell * 9 },
	{ stashX + stashNextCell * 3,  stashY + stashNextCell * 9 },
	{ stashX + stashNextCell * 4,  stashY + stashNextCell * 9 },
	{ stashX + stashNextCell * 5,  stashY + stashNextCell * 9 },
	{ stashX + stashNextCell * 6,  stashY + stashNextCell * 9 },
	{ stashX + stashNextCell * 7,  stashY + stashNextCell * 9 },
	{ stashX + stashNextCell * 8,  stashY + stashNextCell * 9 },
	{ stashX + stashNextCell * 9,  stashY + stashNextCell * 9 }

	// clang-format on
};

int stashNavY = 15; // position for stash buttons
int stashNavW = 21;
int stashNavH = 19;

int stashGoldY = 16; // position for withdraw gold button
int stashGoldW = 27;
int stashGoldH = 19;

const Rectangle StashButtonRect[] = {
	// Contains mappings for the buttons in the stash (2 navigation buttons, withdraw gold buttons, 2 navigation buttons)
	// clang-format off
	//  X,   Y,   W,   H
	{ { 35,   stashNavY },   { stashNavW, stashNavH } }, // 10 left
	{ { 63,   stashNavY },   { stashNavW, stashNavH } }, // 1 left
	{ { 92,  stashGoldY }, { stashGoldW, stashGoldH } }, // withdraw gold
	{ { 240,  stashNavY },   { stashNavW, stashNavH } }, // 1 right
	{ { 268,  stashNavY },   { stashNavW, stashNavH } }  // 10 right

	// clang-format on
};

	int stashCelY = 33;

const Point StashButtonCelRect[] = {
	// clang-format off
	//  X,   Y
	{ 35,   stashCelY }, // 10 left
	{ 63,   stashCelY }, // 1 left
	{ 92,  stashCelY }, // withdraw gold
	{ 240,  stashCelY }, // 1 right
	{ 268,  stashCelY },  // 10 right

	// clang-format on
};

void LoadStash(int page)
{
	// Loads stash from file

	const char *stashHeader;
	if (gbIsSpawn) {
		if (!gbIsMultiplayer)
			stashHeader = "SSSP001"; // Spawn Stash Single Player
		else
			stashHeader = "SSMP001"; // Spawn Stash Multi Player
	} else if (gbIsHellfire) {
		if (!gbIsMultiplayer)
			stashHeader = "HSSP001"; // Hellfire Stash Single Player
		else
			stashHeader = "HSMP001"; // Hellfire Stash Multi Player	
	} else {
		if (!gbIsMultiplayer)
			stashHeader = "DSSP001"; // Diablo Stash Single Player
		else
			stashHeader = "DSMP001"; // Diablo Stash Multi Player	
	}


	// Clear the stash first, in case we fail
	Stash._pNumStash = 0;
	memset(Stash.StashGrid, 0, sizeof(Stash.StashGrid));
	memset(Stash.StashList, 0, sizeof(Stash.StashList));

	std::ifstream fin(GetStashSavePath(page).c_str(), std::ifstream::binary);

	if (fin.is_open()) {
		fin.read((char *)&stashHeader, sizeof(stashHeader));
		fin.read((char *)&Stash._pNumStash, sizeof(Stash._pNumStash));
		fin.read((char *)&Stash.StashGrid, sizeof(Stash.StashGrid));
		for (int i = 0; i < Stash._pNumStash; i++) {
			fin.read((char *)Stash.StashList, static_cast<std::streamsize>(sizeof(Item)) * Stash._pNumStash);
		}
		fin.close();
	}
}

void SaveStash(int page)
{
	// Saves stash to file

	const char *stashHeader = "DXS001";
	if (gbIsHellfire)
		stashHeader = "HDXS001";

	std::ofstream fout(GetStashSavePath(page).c_str(), std::ofstream::binary);

	if (fout.is_open()) {
		fout.write((char *)&stashHeader, sizeof(stashHeader));
		fout.write((char *)&Stash._pNumStash, sizeof(Stash._pNumStash));
		fout.write((char *)&Stash.StashGrid, sizeof(Stash.StashGrid));
		fout.write((char *)Stash.StashList, static_cast<std::streamsize>(sizeof(Item)) * Stash._pNumStash);
		fout.close();
	}
}

namespace {
enum class stash_xy_slot;

std::optional<CelSprite> pStashCels;
std::optional<CelSprite> pStashNavBtnsCels;
std::optional<CelSprite> pStashGoldBtnCels;

void StashDrawSlotBack(const Surface &out, Point targetPosition, Size size)
{
	SDL_Rect srcRect = MakeSdlRect(0, 0, size.width, size.height);
	out.Clip(&srcRect, &targetPosition);
	if (size.width <= 0 || size.height <= 0)
		return;

	std::uint8_t *dst = &out[targetPosition];
	const auto dstPitch = out.pitch();

	for (int hgt = size.height; hgt != 0; hgt--, dst -= dstPitch + size.width) {
		for (int wdt = size.width; wdt != 0; wdt--) {
			std::uint8_t pix = *dst;
			if (pix >= PAL16_BLUE) {
				if (pix <= PAL16_BLUE + 15)
					pix -= PAL16_BLUE - PAL16_BEIGE;
				else if (pix >= PAL16_GRAY)
					pix -= PAL16_GRAY - PAL16_BEIGE;
			}
			*dst++ = pix;
		}
	}
}

/**
 * @brief Adds an item to a player's StashGrid array
 * @param stashGridIndex Item's position in StashGrid (this should be the item's topleft grid tile)
 * @param stashListIndex The item's StashList index (it's expected this already has +1 added to it since StashGrid can't store a 0 index)
 * @param itemSize Size of item
 */
void AddItemToStashGrid(int stashGridIndex, int stashListIndex, Size itemSize)
{
	const int pitch = 10;
	for (int y = 0; y < itemSize.height; y++) {
		for (int x = 0; x < itemSize.width; x++) {
			if (x == 0 && y == itemSize.height - 1)
				Stash.StashGrid[stashGridIndex + x] = stashListIndex;
			else
				Stash.StashGrid[stashGridIndex + x] = -stashListIndex;
		}
		stashGridIndex += pitch;
	}
}

/**
 * @brief Gets the size, in stash cells, of the given item.
 * @param item The item whose size is to be determined.
 * @return The size, in stash cells, of the item.
 */
Size GetStashSize(const Item &item)
{
	int itemSizeIndex = item._iCurs + CURSOR_FIRSTITEM;
	auto size = GetInvItemSize(itemSizeIndex);

	return { size.width / InventorySlotSizeInPixels.width, size.height / InventorySlotSizeInPixels.height };
}

/**
 * @brief Checks whether the given item can fit in a belt slot (i.e. the item's size in inventory cells is 1x1).
 * @param item The item to be checked.
 * @return 'True' in case the item can fit a belt slot and 'False' otherwise.
 */
bool FitsInBeltSlot(const Item &item)
{
	return GetStashSize(item) == Size { 1, 1 };
}

/**
 * @brief Checks whether the given item can be placed on the belt. Takes item size as well as characteristics into account. Items
 * that cannot be placed on the belt have to be placed in the inventory instead.
 * @param item The item to be checked.
 * @return 'True' in case the item can be placed on the belt and 'False' otherwise.
 */
bool CanBePlacedOnBelt(const Item &item)
{
	return FitsInBeltSlot(item)
	    && item._itype != ItemType::Gold
	    && item._iStatFlag
	    && AllItemsList[item.IDidx].iUsable;
}

int SwapItem(Item *a, Item *b)
{
	std::swap(*a, *b);

	return b->_iCurs + CURSOR_FIRSTITEM;
}

void CheckStashPaste(int pnum, Point cursorPosition)
{
	auto &player = Players[pnum];

	if (player.HoldItem._itype == ItemType::Gold) {
		stashGold += player.HoldItem._ivalue;
		player.HoldItem._itype = ItemType::None;
		if (pnum == MyPlayerId) {
			SetCursorPos(MousePosition + Displacement(cursSize / 2));
			NewCursor(CURSOR_HAND);
		}
		return;
	}


	SetICursor(player.HoldItem._iCurs + CURSOR_FIRSTITEM);
	int i = cursorPosition.x + (IsHardwareCursor() ? 0 : (icursSize.width / 2));
	int j = cursorPosition.y + (IsHardwareCursor() ? 0 : (icursSize.height / 2));
	Size itemSize { icursSize28 };
	bool done = false;
	int r = 0;

	if ((itemSize.width & 1) == 0)
		i -= INV_SLOT_HALF_SIZE_PX;
	if ((itemSize.height & 1) == 0)
		j -= INV_SLOT_HALF_SIZE_PX;

	for (; r < STASH_NUM_XY_SLOTS && !done; r++) {
		int xo = GetLeftPanel().position.x;
		int yo = GetLeftPanel().position.y;

		if (i >= StashRect[r].x + xo && i <= StashRect[r].x + xo + InventorySlotSizeInPixels.width) {
			if (j >= StashRect[r].y + yo - InventorySlotSizeInPixels.height - 1 && j < StashRect[r].y + yo) {
				done = true;
				r--;
			}
		}

		if (r == SLOTXY_STASH_LAST && (itemSize.height & 1) == 0)
			j += INV_SLOT_HALF_SIZE_PX;
	}
	if (!done)
		return;

	item_equip_type il = ILOC_UNEQUIPABLE;

	done = player.HoldItem._iLoc == il;

	int8_t it = 0;
	if (il == ILOC_UNEQUIPABLE) {
		done = true;
		int ii = r - SLOTXY_STASH_FIRST;
		if (player.HoldItem._itype == ItemType::Gold) {
			if (Stash.StashGrid[ii] != 0) {
				int8_t iv = Stash.StashGrid[ii];
				if (iv > 0) {
					if (Stash.StashList[iv - 1]._itype != ItemType::Gold) {
						it = iv;
					}
				} else {
					it = -iv;
				}
			}
		} else {
			int yy = std::max(INV_ROW_SLOT_SIZE * ((ii / INV_ROW_SLOT_SIZE) - ((itemSize.height - 1) / 2)), 0);
			for (j = 0; j < itemSize.height && done; j++) {
				if (yy >= NUM_STASH_GRID_ELEM)
					done = false;
				int xx = std::max((ii % INV_ROW_SLOT_SIZE) - ((itemSize.width - 1) / 2), 0);
				for (i = 0; i < itemSize.width && done; i++) {
					if (xx >= INV_ROW_SLOT_SIZE) {
						done = false;
					} else {
						if (Stash.StashGrid[xx + yy] != 0) {
							int8_t iv = abs(Stash.StashGrid[xx + yy]);
							if (it != 0) {
								if (it != iv)
									done = false;
							} else {
								it = iv;
							}
						}
					}
					xx++;
				}
				yy += INV_ROW_SLOT_SIZE;
			}
		}
	}

	if (!done)
		return;

	if (!done)
		return;

	if (pnum == MyPlayerId)
		PlaySFX(ItemInvSnds[ItemCAnimTbl[player.HoldItem._iCurs]]);

	int cn = CURSOR_HAND;
	switch (il) {
	case ILOC_UNEQUIPABLE:
		if (player.HoldItem._itype == ItemType::Gold && it == 0) {
			int ii = r - SLOTXY_STASH_FIRST;
			if (Stash.StashGrid[ii] > 0) {
				int stashIndex = Stash.StashGrid[ii] - 1;
				int gt = Stash.StashList[stashIndex]._ivalue;
				int ig = player.HoldItem._ivalue + gt;
				if (ig <= MaxGold) {
					Stash.StashList[stashIndex]._ivalue = ig;
					player._pGold += player.HoldItem._ivalue;
					SetPlrHandGoldCurs(Stash.StashList[stashIndex]);
				} else {
					ig = MaxGold - gt;
					player._pGold += ig;
					player.HoldItem._ivalue -= ig;
					Stash.StashList[stashIndex]._ivalue = MaxGold;
					Stash.StashList[stashIndex]._iCurs = ICURS_GOLD_LARGE;
					cn = GetGoldCursor(player.HoldItem._ivalue);
					cn += CURSOR_FIRSTITEM;
				}
			} else {
				int stashIndex = Stash._pNumStash;
				Stash.StashList[stashIndex] = player.HoldItem;
				Stash._pNumStash++;
				Stash.StashGrid[ii] = Stash._pNumStash;
				player._pGold += player.HoldItem._ivalue;
				SetPlrHandGoldCurs(Stash.StashList[stashIndex]);
			}
		} else {
			if (it == 0) {
				Stash.StashList[Stash._pNumStash] = player.HoldItem;
				Stash._pNumStash++;
				it = Stash._pNumStash;
			} else {
				int stashIndex = it - 1;
				if (player.HoldItem._itype == ItemType::Gold)
					player._pGold += player.HoldItem._ivalue;
				cn = SwapItem(&Stash.StashList[stashIndex], &player.HoldItem);
				if (player.HoldItem._itype == ItemType::Gold)
					player._pGold = CalculateGold(player);
				for (auto &itemId : Stash.StashGrid) {
					if (itemId == it)
						itemId = 0;
					if (itemId == -it)
						itemId = 0;
				}
			}
			int ii = r - SLOTXY_STASH_FIRST;

			// Calculate top-left position of item for InvGrid and then add item to InvGrid

			int xx = std::max(ii % INV_ROW_SLOT_SIZE - ((itemSize.width - 1) / 2), 0);
			int yy = std::max(INV_ROW_SLOT_SIZE * (ii / INV_ROW_SLOT_SIZE - ((itemSize.height - 1) / 2)), 0);
			AddItemToStashGrid(xx + yy, it, itemSize);
		}
		break;
	case ILOC_NONE:
	case ILOC_INVALID:
		break;
	}
	CalcPlrInv(player, true);
	if (pnum == MyPlayerId) {
		if (cn == CURSOR_HAND && !IsHardwareCursor())
			SetCursorPos(MousePosition + Displacement(cursSize / 2));
		NewCursor(cn);
	}
	SaveStash(Page);
	LoadStash(Page);
}

void CheckStashCut(int pnum, Point cursorPosition, bool automaticMove, bool dropItem)
{
	auto &player = Players[pnum];

	if (player._pmode > PM_WALK3) {
		return;
	}

	if (withdrawGoldFlag) {
		withdrawGoldFlag = false;
		withdrawGoldValue = 0;
	}

	bool done = false;

	uint32_t r = 0;
	for (; r < STASH_NUM_XY_SLOTS; r++) {
		int xo = GetLeftPanel().position.x;
		int yo = GetLeftPanel().position.y;

		// check which inventory rectangle the mouse is in, if any
		if (cursorPosition.x >= StashRect[r].x + xo
		    && cursorPosition.x < StashRect[r].x + xo + (InventorySlotSizeInPixels.width + 1)
		    && cursorPosition.y >= StashRect[r].y + yo - (InventorySlotSizeInPixels.height + 1)
		    && cursorPosition.y < StashRect[r].y + yo) {
			done = true;
			break;
		}
	}

	if (!done) {
		// not on an inventory slot rectangle
		return;
	}

	Item &holdItem = player.HoldItem;
	holdItem._itype = ItemType::None;

	bool automaticallyMoved = false;
	bool automaticallyEquipped = false;
	bool automaticallyUnequip = false;

	if (r >= SLOTXY_STASH_FIRST && r <= SLOTXY_STASH_LAST) {
		int ig = r - SLOTXY_STASH_FIRST;
		int8_t ii = Stash.StashGrid[ig];
		if (ii != 0) {
			int iv = (ii < 0) ? -ii : ii;

			holdItem = Stash.StashList[iv - 1];
			if (automaticMove) {
				if (CanBePlacedOnBelt(holdItem)) {
					automaticallyMoved = AutoPlaceItemInBelt(player, holdItem, true);
				} else {
					automaticallyMoved = automaticallyEquipped = AutoEquip(pnum, holdItem);
				}
			}

			if (!automaticMove || automaticallyMoved) {
				Stash.RemoveStashItem(iv - 1, false);
			}
		}
	}

	if (!holdItem.isEmpty()) {

		CalcPlrInv(player, true);
		CheckItemStats(player);

		if (pnum == MyPlayerId) {
			if (automaticallyEquipped) {
				PlaySFX(ItemInvSnds[ItemCAnimTbl[holdItem._iCurs]]);
			} else if (!automaticMove || automaticallyMoved) {
				PlaySFX(IS_IGRAB);
			}

			if (automaticMove) {
				if (!automaticallyMoved) {
					if (CanBePlacedOnBelt(holdItem) || automaticallyUnequip) {
						player.SaySpecific(HeroSpeech::IHaveNoRoom);
					} else {
						player.SaySpecific(HeroSpeech::ICantDoThat);
					}
				}

				holdItem._itype = ItemType::None;
			} else {
				NewCursor(holdItem._iCurs + CURSOR_FIRSTITEM);
				if (!IsHardwareCursor() && !dropItem) {
					// For a hardware cursor, we set the "hot point" to the center of the item instead.
					SetCursorPos(cursorPosition - Displacement(cursSize / 2));
				}
			}
		}
	}

	if (dropItem) {
		TryDropItem();
	}
	SaveStash(Page);
	LoadStash(Page);
}

void StartGoldWithdraw()
{
	initialWithdrawGoldValue = stashGold;

	auto &myPlayer = Players[MyPlayerId];

	if (talkflag)
		control_reset_talk();

	Point start = GetPanelPosition(UiPanels::Stash, { 67, 128 });
	SDL_Rect rect = MakeSdlRect(start.x, start.y, 180, 20);
	SDL_SetTextInputRect(&rect);

	withdrawGoldFlag = true;
	withdrawGoldValue = 0;
	SDL_StartTextInput();
}

} // namespace

void FreeStashGFX()
{
	pStashCels = std::nullopt;
	pStashNavBtnsCels = std::nullopt;
	pStashGoldBtnCels = std::nullopt;
}

void InitStash()
{
	pStashCels = LoadCel("data\\stash.cel", SPANEL_WIDTH);
	pStashNavBtnsCels = LoadCel("data\\stashnavbtns.cel", 21);
	pStashGoldBtnCels = LoadCel("data\\stashgoldbtn.cel", 27);
	LoadStash(Page);
	stashflag = false;
}

bool StashButtons[5];
bool drawstashbtnflag;
bool stashbtndown;

void SetStashButtonStateDown(int btnId)
{
	StashButtons[btnId] = true;
}

void CheckStashBtnUp()
{
	bool gamemenuOff = true;
	Rectangle stashButton;

	drawstashbtnflag = true;
	stashbtndown = false;

	for (int i = 0; i < 5; i++) {
		if (!StashButtons[i]) {
			continue;
		}

		StashButtons[i] = false;

		stashButton = StashButtonRect[i];
		stashButton.position = GetPanelPosition(UiPanels::Stash, stashButton.position);
		if (!stashButton.Contains(MousePosition)) {
			continue;
		}

		switch (i) {
			case 0:
				Page -= 10;
				break;
			case 1:
				Page -= 1;
				break;
			case 2:
				StartGoldWithdraw();
				break;
			case 3:
				Page += 1;
				break;
			case 4:
				Page += 10;
				break;
		}
	}

	if (Page < 0)
		Page = 0;
	if (Page > 29)
		Page = 29;

	LoadStash(Page);
}

void DoStashBtn()
{
	Rectangle stashButton;

	for (int i = 0; i < 5; i++) {
		stashButton = StashButtonRect[i];
		stashButton.position = GetPanelPosition(UiPanels::Stash, stashButton.position);
		if (stashButton.Contains(MousePosition)) {
			StashButtons[i] = true;
			drawstashbtnflag = true;
			stashbtndown = true;
		}
	}
}

void DrawStash(const Surface &out)
{
	int slot = 0;
	Point stashButton;
	CelDrawTo(out, GetPanelPosition(UiPanels::Stash, { 0, 351 }), *pStashCels, 1);
	for (int penis = 0; penis < 5; penis++) {
		stashButton = StashButtonCelRect[penis];
		if (StashButtons[penis]) {
			switch (penis) {
			case 0:
				CelDrawTo(out, { stashButton.x, stashButton.y }, *pStashNavBtnsCels, 1);
				break;
			case 1:
				CelDrawTo(out, { stashButton.x, stashButton.y }, *pStashNavBtnsCels, 2);
				break;
			case 2:
				CelDrawTo(out, { stashButton.x, stashButton.y }, *pStashGoldBtnCels, 1);
				break;
			case 3:
				CelDrawTo(out, { stashButton.x, stashButton.y }, *pStashNavBtnsCels, 3);
				break;
			case 4:
				CelDrawTo(out, { stashButton.x, stashButton.y }, *pStashNavBtnsCels, 4);
				break;
			}
		}
	}

	for (int i = 0; i < NUM_STASH_GRID_ELEM; i++) {
		if (Stash.StashGrid[i] != 0) {
			StashDrawSlotBack(
			    out,
			    GetPanelPosition(UiPanels::Stash, StashRect[i + SLOTXY_STASH_FIRST]) + Displacement { 0, -1 },
			    InventorySlotSizeInPixels);
		}
	}

	for (int j = 0; j < NUM_STASH_GRID_ELEM; j++) {
		if (Stash.StashGrid[j] > 0) { // first slot of an item
			int ii = Stash.StashGrid[j] - 1;
			int frame = Stash.StashList[ii]._iCurs + CURSOR_FIRSTITEM;

			const auto &cel = GetInvItemSprite(frame);
			const int celFrame = GetInvItemFrame(frame);
			const Point position = GetPanelPosition(UiPanels::Stash, StashRect[j + SLOTXY_STASH_FIRST]) + Displacement { 0, -1 };
			if (pcursstashitem == ii + STASHITEM_STASH_FIRST) {
				CelBlitOutlineTo(
				    out,
				    GetOutlineColor(Stash.StashList[ii], true),
				    position,
				    cel, celFrame, false);
			}

			CelDrawItem(
			    Stash.StashList[ii],
			    out,
			    position,
			    cel, celFrame);
		}
	}

	Point pos = GetPanelPosition(UiPanels::Stash, { 0, 0 });
	for (auto &entry : stashEntries) {
		if (entry.statDisplayFunc != nullptr) {
			StyledText tmp = entry.statDisplayFunc();
			DrawString(
			    out,
			    tmp.text,
			    { entry.position + Displacement { pos.x, pos.y + 3 }, { entry.length, 18 } },
			    UiFlags::AlignCenter | UiFlags::VerticalCenter | tmp.style, tmp.spacing);
		}
	}
}

void CheckStashSwap(Player &player, inv_body_loc bLoc, int idx, uint16_t wCI, int seed, bool bId, uint32_t dwBuff)
{
	auto &item = Items[MAXITEMS];
	memset(&item, 0, sizeof(item));
	RecreateItem(item, idx, wCI, seed, 0, (dwBuff & CF_HELLFIRE) != 0);

	player.HoldItem = item;

	if (bId) {
		player.HoldItem._iIdentified = true;
	}

	player.InvBody[bLoc] = player.HoldItem;

	if (bLoc == INVLOC_HAND_LEFT && player.HoldItem._iLoc == ILOC_TWOHAND) {
		player.InvBody[INVLOC_HAND_RIGHT]._itype = ItemType::None;
	} else if (bLoc == INVLOC_HAND_RIGHT && player.HoldItem._iLoc == ILOC_TWOHAND) {
		player.InvBody[INVLOC_HAND_LEFT]._itype = ItemType::None;
	}

	CalcPlrInv(player, true);
}

void CheckStashItem(bool isShiftHeld, bool isCtrlHeld)
{
	if (pcurs >= CURSOR_FIRSTITEM) {
		CheckStashPaste(MyPlayerId, MousePosition);
	} else {
		CheckStashCut(MyPlayerId, MousePosition, isShiftHeld, isCtrlHeld);
	}
}

int8_t CheckStashHLight()
{
	int8_t r = 0;
	for (; r < STASH_NUM_XY_SLOTS; r++) {
		int xo = GetLeftPanel().position.x;
		int yo = GetLeftPanel().position.y;

		if (MousePosition.x >= StashRect[r].x + xo
		    && MousePosition.x < StashRect[r].x + xo + (InventorySlotSizeInPixels.width + 1)
		    && MousePosition.y >= StashRect[r].y + yo - (InventorySlotSizeInPixels.height + 1)
		    && MousePosition.y < StashRect[r].y + yo) {
			break;
		}
	}

	if (r >= STASH_NUM_XY_SLOTS)
		return -1;

	int8_t rv = -1;
	InfoColor = UiFlags::ColorWhite;
	Item *pi = nullptr;

	ClearPanel();

	if (r >= SLOTXY_STASH_FIRST && r <= SLOTXY_STASH_LAST) {
		int8_t itemId = abs(Stash.StashGrid[r - SLOTXY_STASH_FIRST]);
		if (itemId == 0)
			return -1;
		int ii = itemId - 1;
		rv = ii + STASHITEM_STASH_FIRST;
		pi = &Stash.StashList[ii];
	}

	if (pi->isEmpty())
		return -1;

	if (pi->_itype == ItemType::Gold) {
		int nGold = pi->_ivalue;
		strcpy(infostr, fmt::format(ngettext("{:d} gold piece", "{:d} gold pieces", nGold), nGold).c_str());
	} else {
		InfoColor = pi->getTextColor();
		if (pi->_iIdentified) {
			strcpy(infostr, pi->_iIName);
			PrintItemDetails(*pi);
		} else {
			strcpy(infostr, pi->_iName);
			PrintItemDur(*pi);
		}
	}

	return rv;
}

bool UseStashItem(int pnum, int cii)
{
	int c;
	Item *item;

	auto &player = Players[pnum];

	if (player._pInvincible && player._pHitPoints == 0 && pnum == MyPlayerId)
		return true;
	if (pcurs != CURSOR_HAND)
		return true;
	if (stextflag != STORE_NONE)
		return true;
	if (cii < STASHITEM_STASH_FIRST)
		return false;

	if (cii <= STASHITEM_STASH_LAST) {
		c = cii - STASHITEM_STASH_FIRST;
		item = &Stash.StashList[c];
	}

	constexpr int SpeechDelay = 10;
	if (item->IDidx == IDI_MUSHROOM) {
		player.Say(HeroSpeech::NowThatsOneBigMushroom, SpeechDelay);
		return true;
	}
	if (item->IDidx == IDI_FUNGALTM) {
		PlaySFX(IS_IBOOK);
		player.Say(HeroSpeech::ThatDidntDoAnything, SpeechDelay);
		return true;
	}

	if (!AllItemsList[item->IDidx].iUsable)
		return false;

	if (!item->_iStatFlag) {
		player.Say(HeroSpeech::ICantUseThisYet);
		return true;
	}

	if (withdrawGoldFlag) {
		withdrawGoldFlag = false;
		withdrawGoldValue = 0;
	}

	if (item->IsScroll() && currlevel == 0 && !spelldata[item->_iSpell].sTownSpell) {
		return true;
	}

	if (item->_iMiscId > IMISC_RUNEFIRST && item->_iMiscId < IMISC_RUNELAST && currlevel == 0) {
		return true;
	}

	int idata = ItemCAnimTbl[item->_iCurs];
	if (item->_iMiscId == IMISC_BOOK)
		PlaySFX(IS_RBOOK);
	else if (pnum == MyPlayerId)
		PlaySFX(ItemInvSnds[idata]);

	UseItem(pnum, item->_iMiscId, item->_iSpell);

	if (Stash.StashList[c]._iMiscId == IMISC_MAPOFDOOM)
		return true;
	if (Stash.StashList[c]._iMiscId == IMISC_NOTE) {
		InitQTextMsg(TEXT_BOOK9);
		stashflag = false;
		return true;
	}
	Stash.RemoveStashItem(c, false);

	return true;
}

} // namespace devilution
