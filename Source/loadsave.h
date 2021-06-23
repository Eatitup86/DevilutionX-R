/**
 * @file loadsave.h
 *
 * Interface of save game functionality.
 */
#pragma once

#include "player.h"

namespace devilution {

extern bool gbIsHellfireSaveGame;
extern uint8_t giNumberOfLevels;

void RemoveInvalidItem(ItemStruct *pItem);
int RemapItemIdxFromDiablo(int i);
int RemapItemIdxToDiablo(int i);
bool IsHeaderValid(uint32_t magicNumber);
void LoadHotkeys();
void LoadHeroItems(PlayerStruct &player);
/**
 * @brief Remove invalid inventory items from the inventory grid
 * @param pnum The id of the player
 */
void RemoveEmptyInventory(PlayerStruct &player);
void LoadGame(bool firstflag);
void SaveHotkeys();
void SaveHeroItems(PlayerStruct &player);
void SaveGameData();
void SaveGame();
void SaveLevel();
void LoadLevel();

} // namespace devilution
