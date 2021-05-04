/**
 * @file player.h
 *
 * Interface of player functionality, leveling, actions, creation, loading, etc.
 */
#pragma once

#include <cstdint>

#include "diablo.h"
#include "engine.h"
#include "gendung.h"
#include "items.h"
#include "multi.h"
#include "spelldat.h"
#include "path.h"
#include "interfac.h"
#include "utils/enum_traits.h"
#include "engine/animationinfo.h"

namespace devilution {

// number of inventory grid cells
#define NUM_INV_GRID_ELEM 40
#define MAXBELTITEMS 8
#define MAXRESIST 75
#define MAXCHARLEVEL 51
#define MAX_SPELL_LEVEL 15
#define PLR_NAME_LEN 32

/** Walking directions */
enum {
	// clang-format off
	WALK_NE   =  1,
	WALK_NW   =  2,
	WALK_SE   =  3,
	WALK_SW   =  4,
	WALK_N    =  5,
	WALK_E    =  6,
	WALK_S    =  7,
	WALK_W    =  8,
	WALK_NONE = -1,
	// clang-format on
};

enum class HeroClass : uint8_t {
	Warrior,
	Rogue,
	Sorcerer,
	Monk,
	Bard,
	Barbarian,

	LAST = Barbarian
};

enum class CharacterAttribute : uint8_t {
	Strength,
	Magic,
	Dexterity,
	Vitality,

	FIRST = Strength,
	LAST = Vitality
};

// Logical equipment locations
enum inv_body_loc : uint8_t {
	INVLOC_HEAD,
	INVLOC_RING_LEFT,
	INVLOC_RING_RIGHT,
	INVLOC_AMULET,
	INVLOC_HAND_LEFT,
	INVLOC_HAND_RIGHT,
	INVLOC_CHEST,
	NUM_INVLOC,
};

enum player_graphic : uint16_t {
	// clang-format off
	PFILE_STAND     = 1 << 0,
	PFILE_WALK      = 1 << 1,
	PFILE_ATTACK    = 1 << 2,
	PFILE_HIT       = 1 << 3,
	PFILE_LIGHTNING = 1 << 4,
	PFILE_FIRE      = 1 << 5,
	PFILE_MAGIC     = 1 << 6,
	PFILE_DEATH     = 1 << 7,
	PFILE_BLOCK     = 1 << 8,
	// everything except PFILE_DEATH
	// 0b1_0111_1111
	PFILE_NONDEATH  = 0x17F
	// clang-format on
};

enum anim_weapon_id : uint8_t {
	ANIM_ID_UNARMED,
	ANIM_ID_UNARMED_SHIELD,
	ANIM_ID_SWORD,
	ANIM_ID_SWORD_SHIELD,
	ANIM_ID_BOW,
	ANIM_ID_AXE,
	ANIM_ID_MACE,
	ANIM_ID_MACE_SHIELD,
	ANIM_ID_STAFF,
};

enum PLR_MODE : uint8_t {
	PM_STAND,
	PM_WALK,  //Movement towards N, NW, or NE
	PM_WALK2, //Movement towards S, SW, or SE
	PM_WALK3, //Movement towards W or E
	PM_ATTACK,
	PM_RATTACK,
	PM_BLOCK,
	PM_GOTHIT,
	PM_DEATH,
	PM_SPELL,
	PM_NEWLVL,
	PM_QUIT,
};

enum action_id : int8_t {
	// clang-format off
	ACTION_WALK        = -2, // Automatic walk when using gamepad
	ACTION_NONE        = -1,
	ACTION_ATTACK      = 9,
	ACTION_RATTACK     = 10,
	ACTION_SPELL       = 12,
	ACTION_OPERATE     = 13,
	ACTION_DISARM      = 14,
	ACTION_PICKUPITEM  = 15, // put item in hand (inventory screen open)
	ACTION_PICKUPAITEM = 16, // put item in inventory
	ACTION_TALK        = 17,
	ACTION_OPERATETK   = 18, // operate via telekinesis
	ACTION_ATTACKMON   = 20,
	ACTION_ATTACKPLR   = 21,
	ACTION_RATTACKMON  = 22,
	ACTION_RATTACKPLR  = 23,
	ACTION_SPELLMON    = 24,
	ACTION_SPELLPLR    = 25,
	ACTION_SPELLWALL   = 26,
	// clang-format on
};

enum player_weapon_type : uint8_t {
	WT_MELEE,
	WT_RANGED,
};

struct PlayerStruct {
	PLR_MODE _pmode;
	int8_t walkpath[MAX_PATH_LENGTH];
	bool plractive;
	action_id destAction;
	int destParam1;
	int destParam2;
	direction destParam3;
	int destParam4;
	int plrlevel;
	ActorPosition position;
	direction _pdir; // Direction faced by player (direction enum)
	int _pgfxnum;    // Bitmask indicating what variant of the sprite the player is using. Lower byte define weapon (anim_weapon_id) and higher values define armour (starting with anim_armor_id)
	/*
	* @brief Contains Information for current Animation
	*/
	AnimationInfo AnimInfo;
	int _pAnimWidth;
	int _plid;
	int _pvid;
	spell_id _pSpell;
	spell_type _pSplType;
	int8_t _pSplFrom; // TODO Create enum
	spell_id _pTSpell;
	spell_type _pTSplType;
	spell_id _pRSpell;
	spell_type _pRSplType;
	spell_id _pSBkSpell;
	spell_type _pSBkSplType;
	int8_t _pSplLvl[64];
	uint64_t _pMemSpells;  // Bitmask of learned spells
	uint64_t _pAblSpells;  // Bitmask of abilities
	uint64_t _pScrlSpells; // Bitmask of spells available via scrolls
	uint8_t _pSpellFlags;
	spell_id _pSplHotKey[4];
	spell_type _pSplTHotKey[4];
	player_weapon_type _pwtype;
	bool _pBlockFlag;
	bool _pInvincible;
	int8_t _pLightRad;
	bool _pLvlChanging; // True when the player is transitioning between levels
	char _pName[PLR_NAME_LEN];
	HeroClass _pClass;
	int _pStrength;
	int _pBaseStr;
	int _pMagic;
	int _pBaseMag;
	int _pDexterity;
	int _pBaseDex;
	int _pVitality;
	int _pBaseVit;
	int _pStatPts;
	int _pDamageMod;
	int _pBaseToBlk;
	int _pHPBase;
	int _pMaxHPBase;
	int _pHitPoints;
	int _pMaxHP;
	int _pHPPer;
	int _pManaBase;
	int _pMaxManaBase;
	int _pMana;
	int _pMaxMana;
	int _pManaPer;
	int8_t _pLevel;
	int8_t _pMaxLvl;
	int _pExperience;
	int _pMaxExp;
	int _pNextExper;
	int8_t _pArmorClass;
	int8_t _pMagResist;
	int8_t _pFireResist;
	int8_t _pLghtResist;
	int _pGold;
	bool _pInfraFlag;
	/** Player's direction when ending movement. Also used for casting direction of SPL_FIREWALL. */
	direction tempDirection;
	/** Used for spell level, and X component of _pVar5 */
	int _pVar4;
	/** Used for storing position of a tile which should have its BFLAG_PLAYERLR flag removed after walking. When starting to walk the game places the player in the dPlayer array -1 in the Y coordinate, and uses BFLAG_PLAYERLR to check if it should be using -1 to the Y coordinate when rendering the player (also used for storing the level of a spell when the player casts it) */
	int _pVar5;
	/** Used for counting how close we are to reaching the next tile when walking (usually counts to 8, which is equal to the walk animation length). */
	int actionFrame;
	/** Used for stalling the appearance of the options screen after dying in singleplayer */
	int deathFrame;
	bool _pLvlVisited[NUMLEVELS];
	bool _pSLvlVisited[NUMLEVELS]; // only 10 used
	                               /** Using player_graphic as bitflags */
	int _pGFXLoad;
	uint8_t *_pNAnim[8]; // Stand animations
	int _pNFrames;
	int _pNWidth;
	uint8_t *_pWAnim[8]; // Walk animations
	int _pWFrames;
	int _pWWidth;
	uint8_t *_pAAnim[8]; // Attack animations
	int _pAFrames;
	int _pAWidth;
	int _pAFNum;
	uint8_t *_pLAnim[8]; // Lightning spell cast animations
	uint8_t *_pFAnim[8]; // Fire spell cast animations
	uint8_t *_pTAnim[8]; // Generic spell cast animations
	int _pSFrames;
	int _pSWidth;
	int _pSFNum;
	uint8_t *_pHAnim[8]; // Getting hit animations
	int _pHFrames;
	int _pHWidth;
	uint8_t *_pDAnim[8]; // Death animations
	int _pDFrames;
	int _pDWidth;
	uint8_t *_pBAnim[8]; // Block animations
	int _pBFrames;
	int _pBWidth;
	ItemStruct InvBody[NUM_INVLOC];
	ItemStruct InvList[NUM_INV_GRID_ELEM];
	int _pNumInv;
	int8_t InvGrid[NUM_INV_GRID_ELEM];
	ItemStruct SpdList[MAXBELTITEMS];
	ItemStruct HoldItem;
	int _pIMinDam;
	int _pIMaxDam;
	int _pIAC;
	int _pIBonusDam;
	int _pIBonusToHit;
	int _pIBonusAC;
	int _pIBonusDamMod;
	/** Bitmask of staff spell */
	uint64_t _pISpells;
	/** Bitmask using item_special_effect */
	int _pIFlags;
	int _pIGetHit;
	int8_t _pISplLvlAdd;
	int _pISplDur;
	int _pIEnAc;
	int _pIFMinDam;
	int _pIFMaxDam;
	int _pILMinDam;
	int _pILMaxDam;
	item_misc_id _pOilType;
	uint8_t pTownWarps;
	uint8_t pDungMsgs;
	uint8_t pLvlLoad;
	bool pBattleNet;
	bool pManaShield;
	uint8_t pDungMsgs2;
	bool pOriginalCathedral;
	uint16_t wReflections;
	uint8_t pDiabloKillLevel;
	_difficulty pDifficulty;
	uint32_t pDamAcFlags;
	uint8_t *_pNData;
	uint8_t *_pWData;
	uint8_t *_pAData;
	uint8_t *_pLData;
	uint8_t *_pFData;
	uint8_t *_pTData;
	uint8_t *_pHData;
	uint8_t *_pDData;
	uint8_t *_pBData;

	/**
	 * @brief Gets the most valuable item out of all the player's items that match the given predicate.
	 * @param itemPredicate The predicate used to match the items.
	 * @return The most valuable item out of all the player's items that match the given predicate, or 'nullptr' in case no
	 * matching items were found.
	 */
	template <typename TPredicate>
	const ItemStruct *GetMostValuableItem(const TPredicate &itemPredicate) const
	{
		const auto getMostValuableItem = [&itemPredicate](const ItemStruct *begin, const ItemStruct *end, const ItemStruct *mostValuableItem = nullptr) {
			for (const auto *item = begin; item < end; item++) {
				if (item->isEmpty() || !itemPredicate(*item)) {
					continue;
				}

				if (mostValuableItem == nullptr || item->_iIvalue > mostValuableItem->_iIvalue) {
					mostValuableItem = item;
				}
			}

			return mostValuableItem;
		};

		const ItemStruct *mostValuableItem = getMostValuableItem(SpdList, SpdList + MAXBELTITEMS);
		mostValuableItem = getMostValuableItem(InvBody, InvBody + inv_body_loc::NUM_INVLOC, mostValuableItem);
		mostValuableItem = getMostValuableItem(InvList, InvList + _pNumInv, mostValuableItem);

		return mostValuableItem;
	}

	/**
	 * @brief Gets the base value of the player's specified attribute.
	 * @param attribute The attribute to retrieve the base value for
	 * @return The base value for the requested attribute.
	 */
	int GetBaseAttributeValue(CharacterAttribute attribute) const;

	/**
	 * @brief Gets the maximum value of the player's specified attribute.
	 * @param attribute The attribute to retrieve the maximum value for
	 * @return The maximum value for the requested attribute.
	 */
	int GetMaximumAttributeValue(CharacterAttribute attribute) const;

	/**
	 * @brief Get the tile coordinates a player is moving to (if not moving, then it corresponds to current position).
	 */
	Point GetTargetPosition() const;

	/**
	 * @brief Play a speach file.
	 * @todo Create enum for speachId
	 * @todo BUGFIX Prevent more then one speach to be played at a time (reject new requests).
	 */
	void PlaySpeach(int speachId) const;
	/**
	 * @brief Play a speach file after a given delay.
	 * @param delay Multiple of 20ms waith before starting the speach
	 */
	void PlaySpeach(int speachId, int delay) const;
	/**
	 * @brief Play a player speach file, with out random variants.
	 */
	void PlaySpecificSpeach(int speachId) const;
};

extern int myplr;
extern PlayerStruct plr[MAX_PLRS];
extern bool deathflag;
extern int ToBlkTbl[enum_size<HeroClass>::value];

void LoadPlrGFX(int pnum, player_graphic gfxflag);
void InitPlayerGFX(int pnum);
void InitPlrGFXMem(int pnum);
void FreePlayerGFX(int pnum);

/**
 * @brief Sets the new Player Animation with all relevant information for rendering
 * @param pnum Player Id
 * @param pData Pointer to Animation Data
 * @param numberOfFrames Number of Frames in Animation
 * @param delayLen Delay after each Animation sequence
 * @param width Width of sprite
 * @param params Specifies what special logics are applied to this Animation
 * @param numSkippedFrames Number of Frames that will be skipped (for example with modifier "faster attack")
 * @param distributeFramesBeforeFrame Distribute the numSkippedFrames only before this frame
 */
void NewPlrAnim(int pnum, BYTE *pData, int numberOfFrames, int delayLen, int width, AnimationDistributionParams params = AnimationDistributionParams::None, int numSkippedFrames = 0, int distributeFramesBeforeFrame = 0);
void SetPlrAnims(int pnum);
void CreatePlayer(int pnum, HeroClass c);
int CalcStatDiff(int pnum);
#ifdef _DEBUG
void NextPlrLevel(int pnum);
#endif
void AddPlrExperience(int pnum, int lvl, int exp);
void AddPlrMonstExper(int lvl, int exp, char pmask);
void ApplyPlrDamage(int pnum, int dam, int minHP = 0, int frac = 0, int earflag = 0);
void InitPlayer(int pnum, bool FirstTime);
void InitMultiView();
bool SolidLoc(int x, int y);
void PlrClrTrans(int x, int y);
void PlrDoTrans(int x, int y);
void SetPlayerOld(int pnum);
void FixPlayerLocation(int pnum, direction bDir);
void StartStand(int pnum, direction dir);
void StartAttack(int pnum, direction d);
void StartPlrBlock(int pnum, direction dir);
void FixPlrWalkTags(int pnum);
void RemovePlrFromMap(int pnum);
void StartPlrHit(int pnum, int dam, bool forcehit);
void StartPlayerKill(int pnum, int earflag);
void DropHalfPlayersGold(int pnum);
void StripTopGold(int pnum);
void SyncPlrKill(int pnum, int earflag);
void RemovePlrMissiles(int pnum);
void StartNewLvl(int pnum, interface_mode fom, int lvl);
void RestartTownLvl(int pnum);
void StartWarpLvl(int pnum, int pidx);
void ProcessPlayers();
void ClrPlrPath(PlayerStruct *player);
bool PosOkPlayer(int pnum, int x, int y);
void MakePlrPath(int pnum, int xx, int yy, bool endspace);
void CheckPlrSpell();
void SyncPlrAnim(int pnum);
void SyncInitPlrPos(int pnum);
void SyncInitPlr(int pnum);
void CheckStats(int p);
void ModifyPlrStr(int p, int l);
void ModifyPlrMag(int p, int l);
void ModifyPlrDex(int p, int l);
void ModifyPlrVit(int p, int l);
void SetPlayerHitPoints(int pnum, int val);
void SetPlrStr(int p, int v);
void SetPlrMag(int p, int v);
void SetPlrDex(int p, int v);
void SetPlrVit(int p, int v);
void InitDungMsgs(int pnum);
void PlayDungMsgs();

/* data */

extern int plrxoff[9];
extern int plryoff[9];
extern int plrxoff2[9];
extern int plryoff2[9];
extern int StrengthTbl[enum_size<HeroClass>::value];
extern int MagicTbl[enum_size<HeroClass>::value];
extern int DexterityTbl[enum_size<HeroClass>::value];
extern int VitalityTbl[enum_size<HeroClass>::value];
extern int ExpLvlsTbl[MAXCHARLEVEL];

} // namespace devilution
