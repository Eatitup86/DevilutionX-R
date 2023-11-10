/**
 * @file monstdat.cpp
 *
 * Implementation of all monster data.
 */
#include "monstdat.h"

#include <cstdint>
#include <unordered_map>

#include <expected.hpp>

#include "data/file.hpp"
#include "items.h"
#include "monster.h"
#include "textdat.h"
#include "utils/language.h"

namespace devilution {

namespace {

// Returns a `treasure` value for the given item.
constexpr uint16_t Uniq(_unique_items item)
{
	return static_cast<uint16_t>(T_UNIQ) + item;
}

std::vector<std::string> MonsterSpritePaths;

} // namespace

const char *MonsterData::spritePath() const
{
	return MonsterSpritePaths[static_cast<size_t>(spriteId)].c_str();
}

/** Contains the data related to each monster ID. */
std::vector<MonsterData> MonstersData;

/**
 * Map between .DUN file value and monster type enum
 */
const _monster_id MonstConvTbl[] = {
	MT_NZOMBIE,
	MT_BZOMBIE,
	MT_GZOMBIE,
	MT_YZOMBIE,
	MT_RFALLSP,
	MT_DFALLSP,
	MT_YFALLSP,
	MT_BFALLSP,
	MT_WSKELAX,
	MT_TSKELAX,
	MT_RSKELAX,
	MT_XSKELAX,
	MT_RFALLSD,
	MT_DFALLSD,
	MT_YFALLSD,
	MT_BFALLSD,
	MT_NSCAV,
	MT_BSCAV,
	MT_WSCAV,
	MT_YSCAV,
	MT_WSKELBW,
	MT_TSKELBW,
	MT_RSKELBW,
	MT_XSKELBW,
	MT_WSKELSD,
	MT_TSKELSD,
	MT_RSKELSD,
	MT_XSKELSD,
	MT_SNEAK,
	MT_STALKER,
	MT_UNSEEN,
	MT_ILLWEAV,
	MT_NGOATMC,
	MT_BGOATMC,
	MT_RGOATMC,
	MT_GGOATMC,
	MT_FIEND,
	MT_GLOOM,
	MT_BLINK,
	MT_FAMILIAR,
	MT_NGOATBW,
	MT_BGOATBW,
	MT_RGOATBW,
	MT_GGOATBW,
	MT_NACID,
	MT_RACID,
	MT_BACID,
	MT_XACID,
	MT_SKING,
	MT_FAT,
	MT_MUDMAN,
	MT_TOAD,
	MT_FLAYED,
	MT_WYRM,
	MT_CAVSLUG,
	MT_DEVOUR,
	MT_DVLWYRM,
	MT_NMAGMA,
	MT_YMAGMA,
	MT_BMAGMA,
	MT_WMAGMA,
	MT_HORNED,
	MT_MUDRUN,
	MT_FROSTC,
	MT_OBLORD,
	MT_BONEDMN,
	MT_REDDTH,
	MT_LTCHDMN,
	MT_UDEDBLRG,
	MT_INVALID,
	MT_INVALID,
	MT_INVALID,
	MT_INVALID,
	MT_INCIN,
	MT_FLAMLRD,
	MT_DOOMFIRE,
	MT_HELLBURN,
	MT_INVALID,
	MT_INVALID,
	MT_INVALID,
	MT_INVALID,
	MT_RSTORM,
	MT_STORM,
	MT_STORML,
	MT_MAEL,
	MT_WINGED,
	MT_GARGOYLE,
	MT_BLOODCLW,
	MT_DEATHW,
	MT_MEGA,
	MT_GUARD,
	MT_VTEXLRD,
	MT_BALROG,
	MT_NSNAKE,
	MT_RSNAKE,
	MT_GSNAKE,
	MT_BSNAKE,
	MT_NBLACK,
	MT_RTBLACK,
	MT_BTBLACK,
	MT_RBLACK,
	MT_UNRAV,
	MT_HOLOWONE,
	MT_PAINMSTR,
	MT_REALWEAV,
	MT_SUCCUBUS,
	MT_SNOWWICH,
	MT_HLSPWN,
	MT_SOLBRNR,
	MT_COUNSLR,
	MT_MAGISTR,
	MT_CABALIST,
	MT_ADVOCATE,
	MT_INVALID,
	MT_DIABLO,
	MT_INVALID,
	MT_GOLEM,
	MT_INVALID,
	MT_INVALID,
	MT_INVALID, // Monster from blood1.dun and blood2.dun
	MT_INVALID,
	MT_INVALID,
	MT_INVALID,
	MT_INVALID, // Snotspill from banner2.dun
	MT_INVALID,
	MT_INVALID,
	MT_BIGFALL,
	MT_DARKMAGE,
	MT_HELLBOAR,
	MT_STINGER,
	MT_PSYCHORB,
	MT_ARACHNON,
	MT_FELLTWIN,
	MT_HORKSPWN,
	MT_VENMTAIL,
	MT_NECRMORB,
	MT_SPIDLORD,
	MT_LASHWORM,
	MT_TORCHANT,
	MT_HORKDMN,
	MT_DEFILER,
	MT_GRAVEDIG,
	MT_TOMBRAT,
	MT_FIREBAT,
	MT_SKLWING,
	MT_LICH,
	MT_CRYPTDMN,
	MT_HELLBAT,
	MT_BONEDEMN,
	MT_LICH,
	MT_BICLOPS,
	MT_FLESTHNG,
	MT_REAPER,
	MT_NAKRUL,
	MT_CLEAVER,
	MT_INVILORD,
	MT_LRDSAYTR,
};

tl::expected<MonsterAvailability, std::string> ParseMonsterAvailability(std::string_view value)
{
	if (value == "Always")
		return MonsterAvailability::Always;
	if (value == "Never")
		return MonsterAvailability::Never;
	if (value == "Retail")
		return MonsterAvailability::Retail;
	return tl::make_unexpected("Expected one of: Always, Never, or Retail");
}

tl::expected<MonsterAIID, std::string> ParseAiId(std::string_view value)
{
	if (value == "Zombie")
		return MonsterAIID::Zombie;
	if (value == "Fat")
		return MonsterAIID::Fat;
	if (value == "SkeletonMelee")
		return MonsterAIID::SkeletonMelee;
	if (value == "SkeletonRanged")
		return MonsterAIID::SkeletonRanged;
	if (value == "Scavenger")
		return MonsterAIID::Scavenger;
	if (value == "Rhino")
		return MonsterAIID::Rhino;
	if (value == "GoatMelee")
		return MonsterAIID::GoatMelee;
	if (value == "GoatRanged")
		return MonsterAIID::GoatRanged;
	if (value == "Fallen")
		return MonsterAIID::Fallen;
	if (value == "Magma")
		return MonsterAIID::Magma;
	if (value == "SkeletonKing")
		return MonsterAIID::SkeletonKing;
	if (value == "Bat")
		return MonsterAIID::Bat;
	if (value == "Gargoyle")
		return MonsterAIID::Gargoyle;
	if (value == "Butcher")
		return MonsterAIID::Butcher;
	if (value == "Succubus")
		return MonsterAIID::Succubus;
	if (value == "Sneak")
		return MonsterAIID::Sneak;
	if (value == "Storm")
		return MonsterAIID::Storm;
	if (value == "FireMan")
		return MonsterAIID::FireMan;
	if (value == "Gharbad")
		return MonsterAIID::Gharbad;
	if (value == "Acid")
		return MonsterAIID::Acid;
	if (value == "AcidUnique")
		return MonsterAIID::AcidUnique;
	if (value == "Golem")
		return MonsterAIID::Golem;
	if (value == "Zhar")
		return MonsterAIID::Zhar;
	if (value == "Snotspill")
		return MonsterAIID::Snotspill;
	if (value == "Snake")
		return MonsterAIID::Snake;
	if (value == "Counselor")
		return MonsterAIID::Counselor;
	if (value == "Mega")
		return MonsterAIID::Mega;
	if (value == "Diablo")
		return MonsterAIID::Diablo;
	if (value == "Lazarus")
		return MonsterAIID::Lazarus;
	if (value == "LazarusSuccubus")
		return MonsterAIID::LazarusSuccubus;
	if (value == "Lachdanan")
		return MonsterAIID::Lachdanan;
	if (value == "Warlord")
		return MonsterAIID::Warlord;
	if (value == "FireBat")
		return MonsterAIID::FireBat;
	if (value == "Torchant")
		return MonsterAIID::Torchant;
	if (value == "HorkDemon")
		return MonsterAIID::HorkDemon;
	if (value == "Lich")
		return MonsterAIID::Lich;
	if (value == "ArchLich")
		return MonsterAIID::ArchLich;
	if (value == "Psychorb")
		return MonsterAIID::Psychorb;
	if (value == "Necromorb")
		return MonsterAIID::Necromorb;
	if (value == "BoneDemon")
		return MonsterAIID::BoneDemon;
	return tl::make_unexpected("Unknown enum value");
}

tl::expected<monster_flag, std::string> ParseMonsterFlag(std::string_view value)
{
	if (value == "HIDDEN")
		return MFLAG_HIDDEN;
	if (value == "LOCK_ANIMATION")
		return MFLAG_LOCK_ANIMATION;
	if (value == "ALLOW_SPECIAL")
		return MFLAG_ALLOW_SPECIAL;
	if (value == "TARGETS_MONSTER")
		return MFLAG_TARGETS_MONSTER;
	if (value == "GOLEM")
		return MFLAG_GOLEM;
	if (value == "QUEST_COMPLETE")
		return MFLAG_QUEST_COMPLETE;
	if (value == "KNOCKBACK")
		return MFLAG_KNOCKBACK;
	if (value == "SEARCH")
		return MFLAG_SEARCH;
	if (value == "CAN_OPEN_DOOR")
		return MFLAG_CAN_OPEN_DOOR;
	if (value == "NO_ENEMY")
		return MFLAG_NO_ENEMY;
	if (value == "BERSERK")
		return MFLAG_BERSERK;
	if (value == "NOLIFESTEAL")
		return MFLAG_NOLIFESTEAL;
	return tl::make_unexpected("Unknown enum value");
}

tl::expected<MonsterClass, std::string> ParseMonsterClass(std::string_view value)
{
	if (value == "Undead")
		return MonsterClass::Undead;
	if (value == "Demon")
		return MonsterClass::Demon;
	if (value == "Animal")
		return MonsterClass::Animal;
	return tl::make_unexpected("Unknown enum value");
}

tl::expected<monster_resistance, std::string> ParseMonsterResistance(std::string_view value)
{
	if (value == "RESIST_MAGIC")
		return RESIST_MAGIC;
	if (value == "RESIST_FIRE")
		return RESIST_FIRE;
	if (value == "RESIST_LIGHTNING")
		return RESIST_LIGHTNING;
	if (value == "IMMUNE_MAGIC")
		return IMMUNE_MAGIC;
	if (value == "IMMUNE_FIRE")
		return IMMUNE_FIRE;
	if (value == "IMMUNE_LIGHTNING")
		return IMMUNE_LIGHTNING;
	if (value == "IMMUNE_ACID")
		return IMMUNE_ACID;
	return tl::make_unexpected("Unknown enum value");
}

void LoadMonsterData()
{
	const std::string_view filename = "txtdata\\monsters\\monstdat.tsv";
	tl::expected<DataFile, DataFile::Error> dataFileResult = DataFile::load(filename);
	if (!dataFileResult.has_value()) {
		DataFile::reportFatalError(dataFileResult.error(), filename);
	}

	DataFile &dataFile = dataFileResult.value();
	if (tl::expected<void, DataFile::Error> result = dataFile.skipHeader();
	    !result.has_value()) {
		DataFile::reportFatalError(result.error(), filename);
	}

	MonstersData.clear();
	std::unordered_map<std::string, size_t> spritePathToId;
	for (DataFileRecord record : dataFile) {
		FieldIterator fieldIt = record.begin();
		const FieldIterator endField = record.end();

		MonstersData.emplace_back();
		MonsterData &monster = MonstersData.back();

		const auto advance = [&]() {
			++fieldIt;
			if (fieldIt == endField) {
				DataFile::reportFatalError(DataFile::Error::NotEnoughColumns, filename);
			}
		};

		// Skip the first column (monster ID).

		// name
		advance();
		monster.name = (*fieldIt).value();

		// assetsSuffix
		advance();
		{
			std::string assetsSuffix { (*fieldIt).value() };
			const auto [it, inserted] = spritePathToId.emplace(assetsSuffix, spritePathToId.size());
			if (inserted)
				MonsterSpritePaths.push_back(it->first);
			monster.spriteId = it->second;
		}

		// soundSuffix
		advance();
		monster.soundSuffix = (*fieldIt).value();

		// trnFile
		advance();
		monster.trnFile = (*fieldIt).value();

		// availability
		advance();
		if (tl::expected<MonsterAvailability, std::string> result = ParseMonsterAvailability((*fieldIt).value()); result.has_value()) {
			monster.availability = *std::move(result);
		} else {
			DataFile::reportFatalFieldError(DataFileField::Error::InvalidValue, filename, "availability", *fieldIt, result.error());
		}

		// width
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.width); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "width", *fieldIt);
		}

		// image
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.image); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "image", *fieldIt);
		}

		// hasSpecial
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseBool(monster.hasSpecial); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "hasSpecial", *fieldIt);
		}

		// hasSpecialSound
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseBool(monster.hasSpecialSound); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "hasSpecialSound", *fieldIt);
		}

		// frames[6]
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseIntArray(monster.frames); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "frames", *fieldIt);
		}

		// rate[6]
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseIntArray(monster.rate); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "rate", *fieldIt);
		}

		// minDunLvl
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.minDunLvl); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "minDunLvl", *fieldIt);
		}

		// maxDunLvl
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.maxDunLvl); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "maxDunLvl", *fieldIt);
		}

		// level
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.level); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "level", *fieldIt);
		}

		// hitPointsMinimum
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.hitPointsMinimum); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "hitPointsMinimum", *fieldIt);
		}

		// hitPointsMaximum
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.hitPointsMaximum); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "hitPointsMaximum", *fieldIt);
		}

		// ai
		advance();
		if (tl::expected<MonsterAIID, std::string> result = ParseAiId((*fieldIt).value()); result.has_value()) {
			monster.ai = *std::move(result);
		} else {
			DataFile::reportFatalFieldError(DataFileField::Error::InvalidValue, filename, "ai", *fieldIt, result.error());
		}

		// abilityFlags
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseEnumList(monster.abilityFlags, ParseMonsterFlag); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "abilityFlags", *fieldIt);
		}

		// intelligence
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.intelligence); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "intelligence", *fieldIt);
		}

		// toHit
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.toHit); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "toHit", *fieldIt);
		}

		// animFrameNum
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.animFrameNum); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "animFrameNum", *fieldIt);
		}

		// minDamage
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.minDamage); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "minDamage", *fieldIt);
		}

		// maxDamage
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.maxDamage); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "maxDamage", *fieldIt);
		}

		// toHitSpecial
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.toHitSpecial); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "toHitSpecial", *fieldIt);
		}

		// animFrameNumSpecial
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.animFrameNumSpecial); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "animFrameNumSpecial", *fieldIt);
		}

		// minDamageSpecial
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.minDamageSpecial); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "minDamageSpecial", *fieldIt);
		}

		// maxDamageSpecial
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.maxDamageSpecial); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "maxDamageSpecial", *fieldIt);
		}

		// armorClass
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.armorClass); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "armorClass", *fieldIt);
		}

		// monsterClass
		advance();
		if (tl::expected<MonsterClass, std::string> result = ParseMonsterClass((*fieldIt).value()); result.has_value()) {
			monster.monsterClass = *std::move(result);
		} else {
			DataFile::reportFatalFieldError(DataFileField::Error::InvalidValue, filename, "monsterClass", *fieldIt, result.error());
		}

		// resistance
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseEnumList(monster.resistance, ParseMonsterResistance); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "resistance", *fieldIt);
		}

		// resistanceHell
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseEnumList(monster.resistanceHell, ParseMonsterResistance); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "resistanceHell", *fieldIt);
		}

		// selectionType
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.selectionType); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "selectionType", *fieldIt);
		}

		// treasure
		// TODO: Replace this hack with proper parsing once unique monsters have been migrated.
		advance();
		{
			const std::string_view value = (*fieldIt).value();
			if (value.empty()) {
				monster.treasure = 0;
			} else if (value == "None") {
				monster.treasure = T_NODROP;
			} else if (value == "Uniq(SKCROWN)") {
				monster.treasure = Uniq(UITEM_SKCROWN);
			} else if (value == "Uniq(CLEAVER)") {
				monster.treasure = Uniq(UITEM_CLEAVER);
			} else {
				DataFile::reportFatalFieldError(DataFileField::Error::InvalidValue, filename, "treasure", *fieldIt, "NOTE: Parser is incomplete");
			}
		}

		// exp
		advance();
		if (tl::expected<void, DataFileField::Error> result = (*fieldIt).parseInt(monster.exp); !result.has_value()) {
			DataFile::reportFatalFieldError(result.error(), filename, "exp", *fieldIt);
		}
	}
}

size_t GetNumMonsterSprites()
{
	return MonsterSpritePaths.size();
}

/** Contains the data related to each unique monster ID. */
const UniqueMonsterData UniqueMonstersData[] = {
	// clang-format off
// mtype,      mName,                                     mTrnName,   mlevel,  mmaxhp, mAi,                           mint,  mMinDamage,  mMaxDamage, mMagicRes,                                      monsterPack,                     customToHit,  customArmorClass, mtalkmsg
	// TRANSLATORS: Unique Monster Block start
{ MT_NGOATMC,  P_("monster", "Gharbad the Weak"),         "bsdb",          4,     120, MonsterAIID::Gharbad,             3,           8,          16, IMMUNE_LIGHTNING,                               UniqueMonsterPack::None,                   0,                 0, TEXT_GARBUD1  },
{ MT_SKING,    P_("monster", "Skeleton King"),            "genrl",         0,     240, MonsterAIID::SkeletonKing,        3,           6,          16, IMMUNE_MAGIC | RESIST_FIRE | RESIST_LIGHTNING,  UniqueMonsterPack::Independent,            0,                 0, TEXT_NONE     },
{ MT_COUNSLR,  P_("monster", "Zhar the Mad"),             "general",       8,     360, MonsterAIID::Zhar,                3,          16,          40, IMMUNE_MAGIC | RESIST_FIRE | RESIST_LIGHTNING,  UniqueMonsterPack::None,                   0,                 0, TEXT_ZHAR1    },
{ MT_BFALLSP,  P_("monster", "Snotspill"),                "bng",           4,     220, MonsterAIID::Snotspill,           3,          10,          18, RESIST_LIGHTNING,                               UniqueMonsterPack::None,                   0,                 0, TEXT_BANNER10 },
{ MT_ADVOCATE, P_("monster", "Arch-Bishop Lazarus"),      "general",       0,     600, MonsterAIID::Lazarus,             3,          30,          50, IMMUNE_MAGIC | RESIST_FIRE | RESIST_LIGHTNING,  UniqueMonsterPack::None,                   0,                 0, TEXT_VILE13   },
{ MT_HLSPWN,   P_("monster", "Red Vex"),                  "redv",          0,     400, MonsterAIID::LazarusSuccubus,     3,          30,          50, IMMUNE_MAGIC | RESIST_FIRE,                     UniqueMonsterPack::None,                   0,                 0, TEXT_VILE13   },
{ MT_HLSPWN,   P_("monster", "Black Jade"),               "blkjd",         0,     400, MonsterAIID::LazarusSuccubus,     3,          30,          50, IMMUNE_MAGIC |               RESIST_LIGHTNING,  UniqueMonsterPack::None,                   0,                 0, TEXT_VILE13   },
{ MT_RBLACK,   P_("monster", "Lachdanan"),                "bhka",         14,     500, MonsterAIID::Lachdanan,           3,           0,           0, 0,                                              UniqueMonsterPack::None,                   0,                 0, TEXT_VEIL9    },
{ MT_BTBLACK,  P_("monster", "Warlord of Blood"),         "general",      13,     850, MonsterAIID::Warlord,             3,          35,          50, IMMUNE_MAGIC | IMMUNE_FIRE | IMMUNE_LIGHTNING,  UniqueMonsterPack::None,                   0,                 0, TEXT_WARLRD9  },
{ MT_CLEAVER,  P_("monster", "The Butcher"),              "genrl",         0,     220, MonsterAIID::Butcher,             3,           6,          12, RESIST_FIRE | RESIST_LIGHTNING,                 UniqueMonsterPack::None,                   0,                 0, TEXT_NONE     },
{ MT_HORKDMN,  P_("monster", "Hork Demon"),               "genrl",        19,     300, MonsterAIID::HorkDemon,           3,          20,          35, RESIST_LIGHTNING,                               UniqueMonsterPack::None,                   0,                 0, TEXT_NONE     },
{ MT_DEFILER,  P_("monster", "The Defiler"),              "genrl",        20,     480, MonsterAIID::SkeletonMelee,       3,          30,          40, RESIST_MAGIC | RESIST_FIRE | IMMUNE_LIGHTNING,  UniqueMonsterPack::None,                   0,                 0, TEXT_NONE     },
{ MT_NAKRUL,   P_("monster", "Na-Krul"),                  "genrl",         0,    1332, MonsterAIID::SkeletonMelee,       3,          40,          50, IMMUNE_MAGIC | IMMUNE_FIRE | IMMUNE_LIGHTNING,  UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_TSKELAX,  P_("monster", "Bonehead Keenaxe"),         "bhka",          2,      91, MonsterAIID::SkeletonMelee,       2,           4,          10, IMMUNE_MAGIC,                                   UniqueMonsterPack::Leashed,              100,                 0, TEXT_NONE     },
{ MT_RFALLSD,  P_("monster", "Bladeskin the Slasher"),    "bsts",          2,      51, MonsterAIID::Fallen,              0,           6,          18, RESIST_FIRE,                                    UniqueMonsterPack::Leashed,                0,                45, TEXT_NONE     },
{ MT_NZOMBIE,  P_("monster", "Soulpus"),                  "general",       2,     133, MonsterAIID::Zombie,              0,           4,           8, RESIST_FIRE | RESIST_LIGHTNING,                 UniqueMonsterPack::None,                   0,                 0, TEXT_NONE     },
{ MT_RFALLSP,  P_("monster", "Pukerat the Unclean"),      "ptu",           2,      77, MonsterAIID::Fallen,              3,           1,           5, RESIST_FIRE,                                    UniqueMonsterPack::None,                   0,                 0, TEXT_NONE     },
{ MT_WSKELAX,  P_("monster", "Boneripper"),               "br",            2,      54, MonsterAIID::Bat,                 0,           6,          15, IMMUNE_MAGIC | IMMUNE_FIRE,                     UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_NZOMBIE,  P_("monster", "Rotfeast the Hungry"),      "eth",           2,      85, MonsterAIID::SkeletonMelee,       3,           4,          12, IMMUNE_MAGIC,                                   UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_DFALLSD,  P_("monster", "Gutshank the Quick"),       "gtq",           3,      66, MonsterAIID::Bat,                 2,           6,          16, RESIST_FIRE,                                    UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_TSKELSD,  P_("monster", "Brokenhead Bangshield"),    "bhbs",          3,     108, MonsterAIID::SkeletonMelee,       3,          12,          20, IMMUNE_MAGIC |               RESIST_LIGHTNING,  UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_YFALLSP,  P_("monster", "Bongo"),                    "bng",           3,     178, MonsterAIID::Fallen,              3,           9,          21, 0,                                              UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_BZOMBIE,  P_("monster", "Rotcarnage"),               "rcrn",          3,     102, MonsterAIID::Zombie,              3,           9,          24, IMMUNE_MAGIC |               RESIST_LIGHTNING,  UniqueMonsterPack::Leashed,                0,                45, TEXT_NONE     },
{ MT_NSCAV,    P_("monster", "Shadowbite"),               "shbt",          2,      60, MonsterAIID::SkeletonMelee,       3,           3,          20, IMMUNE_FIRE,                                    UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_WSKELBW,  P_("monster", "Deadeye"),                  "de",            2,      49, MonsterAIID::GoatRanged,          0,           6,           9, IMMUNE_MAGIC | RESIST_FIRE,                     UniqueMonsterPack::None,                   0,                 0, TEXT_NONE     },
{ MT_RSKELAX,  P_("monster", "Madeye the Dead"),          "mtd",           4,      75, MonsterAIID::Bat,                 0,           9,          21, IMMUNE_MAGIC | IMMUNE_FIRE,                     UniqueMonsterPack::Leashed,                0,                30, TEXT_NONE     },
{ MT_BSCAV,    P_("monster", "El Chupacabras"),           "general",       3,     120, MonsterAIID::GoatMelee,           0,          10,          18, RESIST_FIRE,                                    UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_TSKELBW,  P_("monster", "Skullfire"),                "skfr",          3,     125, MonsterAIID::GoatRanged,          1,           6,          10, IMMUNE_FIRE,                                    UniqueMonsterPack::None,                   0,                 0, TEXT_NONE     },
{ MT_SNEAK,    P_("monster", "Warpskull"),                "tspo",          3,     117, MonsterAIID::Sneak,               2,           6,          18, RESIST_FIRE | RESIST_LIGHTNING,                 UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_GZOMBIE,  P_("monster", "Goretongue"),               "pmr",           3,     156, MonsterAIID::SkeletonMelee,       1,          15,          30, IMMUNE_MAGIC,                                   UniqueMonsterPack::None,                   0,                 0, TEXT_NONE     },
{ MT_WSCAV,    P_("monster", "Pulsecrawler"),             "bhka",          4,     150, MonsterAIID::Scavenger,           0,          16,          20, IMMUNE_FIRE | RESIST_LIGHTNING,                 UniqueMonsterPack::Leashed,                0,                45, TEXT_NONE     },
{ MT_BLINK,    P_("monster", "Moonbender"),               "general",       4,     135, MonsterAIID::Bat,                 0,           9,          27, IMMUNE_FIRE,                                    UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_BLINK,    P_("monster", "Wrathraven"),               "general",       5,     135, MonsterAIID::Bat,                 2,           9,          22, IMMUNE_FIRE,                                    UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_YSCAV,    P_("monster", "Spineeater"),               "general",       4,     180, MonsterAIID::Scavenger,           1,          18,          25, IMMUNE_LIGHTNING,                               UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_RSKELBW,  P_("monster", "Blackash the Burning"),     "bashtb",        4,     120, MonsterAIID::GoatRanged,          0,           6,          16, IMMUNE_MAGIC | IMMUNE_FIRE,                     UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_BFALLSD,  P_("monster", "Shadowcrow"),               "general",       5,     270, MonsterAIID::Sneak,               2,          12,          25, 0,                                              UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_LRDSAYTR, P_("monster", "Blightstone the Weak"),     "bhka",          4,     360, MonsterAIID::SkeletonMelee,       0,           4,          12, IMMUNE_MAGIC |               RESIST_LIGHTNING,  UniqueMonsterPack::Leashed,               70,                 0, TEXT_NONE     },
{ MT_FAT,      P_("monster", "Bilefroth the Pit Master"), "bftp",          6,     210, MonsterAIID::Bat,                 1,          16,          23, IMMUNE_MAGIC | IMMUNE_FIRE | RESIST_LIGHTNING,  UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_NGOATBW,  P_("monster", "Bloodskin Darkbow"),        "bsdb",          5,     207, MonsterAIID::GoatRanged,          0,           3,          16, RESIST_FIRE | RESIST_LIGHTNING,                 UniqueMonsterPack::Leashed,                0,                55, TEXT_NONE     },
{ MT_GLOOM,    P_("monster", "Foulwing"),                 "db",            5,     246, MonsterAIID::Rhino,               3,          12,          28, RESIST_FIRE,                                    UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_XSKELSD,  P_("monster", "Shadowdrinker"),            "shdr",          5,     300, MonsterAIID::Sneak,               1,          18,          26, IMMUNE_MAGIC | RESIST_FIRE | RESIST_LIGHTNING,  UniqueMonsterPack::None,                   0,                45, TEXT_NONE     },
{ MT_UNSEEN,   P_("monster", "Hazeshifter"),              "bhka",          5,     285, MonsterAIID::Sneak,               3,          18,          30, IMMUNE_LIGHTNING,                               UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_NACID,    P_("monster", "Deathspit"),                "bfds",          6,     303, MonsterAIID::AcidUnique,          0,          12,          32, RESIST_FIRE | RESIST_LIGHTNING,                 UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_RGOATMC,  P_("monster", "Bloodgutter"),              "bgbl",          6,     315, MonsterAIID::Bat,                 1,          24,          34, IMMUNE_FIRE,                                    UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_BGOATMC,  P_("monster", "Deathshade Fleshmaul"),     "dsfm",          6,     276, MonsterAIID::Rhino,               0,          12,          24, IMMUNE_MAGIC | RESIST_FIRE,                     UniqueMonsterPack::None,                   0,                65, TEXT_NONE     },
{ MT_WYRM,     P_("monster", "Warmaggot the Mad"),        "general",       6,     246, MonsterAIID::Bat,                 3,          15,          30, RESIST_LIGHTNING,                               UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_STORM,    P_("monster", "Glasskull the Jagged"),     "bhka",          7,     354, MonsterAIID::Storm,               0,          18,          30, IMMUNE_MAGIC | IMMUNE_FIRE,                     UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_RGOATBW,  P_("monster", "Blightfire"),               "blf",           7,     321, MonsterAIID::Succubus,            2,          13,          21, IMMUNE_FIRE,                                    UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_GARGOYLE, P_("monster", "Nightwing the Cold"),       "general",       7,     342, MonsterAIID::Bat,                 1,          18,          26, IMMUNE_MAGIC |               RESIST_LIGHTNING,  UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_GGOATBW,  P_("monster", "Gorestone"),                "general",       7,     303, MonsterAIID::GoatRanged,          1,          15,          28, RESIST_LIGHTNING,                               UniqueMonsterPack::Leashed,               70,                 0, TEXT_NONE     },
{ MT_BMAGMA,   P_("monster", "Bronzefist Firestone"),     "general",       8,     360, MonsterAIID::Magma,               0,          30,          36, IMMUNE_MAGIC | RESIST_FIRE,                     UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_INCIN,    P_("monster", "Wrathfire the Doomed"),     "wftd",          8,     270, MonsterAIID::SkeletonMelee,       2,          20,          30, IMMUNE_MAGIC | RESIST_FIRE |  RESIST_LIGHTNING, UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_NMAGMA,   P_("monster", "Firewound the Grim"),       "bhka",          8,     303, MonsterAIID::Magma,               0,          18,          22, IMMUNE_MAGIC | RESIST_FIRE,                     UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_MUDMAN,   P_("monster", "Baron Sludge"),             "bsm",           8,     315, MonsterAIID::Sneak,               3,          25,          34, IMMUNE_MAGIC | RESIST_FIRE | RESIST_LIGHTNING,  UniqueMonsterPack::Leashed,                0,                75, TEXT_NONE     },
{ MT_GGOATMC,  P_("monster", "Blighthorn Steelmace"),     "bhsm",          7,     250, MonsterAIID::Rhino,               0,          20,          28, RESIST_LIGHTNING,                               UniqueMonsterPack::Leashed,                0,                45, TEXT_NONE     },
{ MT_RACID,    P_("monster", "Chaoshowler"),              "general",       8,     240, MonsterAIID::AcidUnique,          0,          12,          20, 0,                                              UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_REDDTH,   P_("monster", "Doomgrin the Rotting"),     "general",       8,     405, MonsterAIID::Storm,               3,          25,          50, IMMUNE_MAGIC | RESIST_FIRE | RESIST_LIGHTNING,  UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_FLAMLRD,  P_("monster", "Madburner"),                "general",       9,     270, MonsterAIID::Storm,               0,          20,          40, IMMUNE_MAGIC | IMMUNE_FIRE | IMMUNE_LIGHTNING,  UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_LTCHDMN,  P_("monster", "Bonesaw the Litch"),        "general",       9,     495, MonsterAIID::Storm,               2,          30,          55, IMMUNE_MAGIC | RESIST_FIRE | RESIST_LIGHTNING,  UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_MUDRUN,   P_("monster", "Breakspine"),               "general",       9,     351, MonsterAIID::Rhino,               0,          25,          34, RESIST_FIRE,                                    UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_REDDTH,   P_("monster", "Devilskull Sharpbone"),     "general",       9,     444, MonsterAIID::Storm,               1,          25,          40, IMMUNE_FIRE,                                    UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_STORM,    P_("monster", "Brokenstorm"),              "general",       9,     411, MonsterAIID::Storm,               2,          25,          36, IMMUNE_LIGHTNING,                               UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_RSTORM,   P_("monster", "Stormbane"),                "general",       9,     555, MonsterAIID::Storm,               3,          30,          30, IMMUNE_LIGHTNING,                               UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_TOAD,     P_("monster", "Oozedrool"),                "general",       9,     483, MonsterAIID::Fat,                 3,          25,          30, RESIST_LIGHTNING,                               UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_BLOODCLW, P_("monster", "Goldblight of the Flame"),  "general",      10,     405, MonsterAIID::Gargoyle,            0,          15,          35, IMMUNE_MAGIC | IMMUNE_FIRE,                     UniqueMonsterPack::Leashed,                0,                80, TEXT_NONE     },
{ MT_OBLORD,   P_("monster", "Blackstorm"),               "general",      10,     525, MonsterAIID::Rhino,               3,          20,          40, IMMUNE_MAGIC |               IMMUNE_LIGHTNING,  UniqueMonsterPack::Leashed,                0,                90, TEXT_NONE     },
{ MT_RACID,    P_("monster", "Plaguewrath"),              "general",      10,     450, MonsterAIID::AcidUnique,          2,          20,          30, IMMUNE_MAGIC | RESIST_FIRE,                     UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_RSTORM,   P_("monster", "The Flayer"),               "general",      10,     501, MonsterAIID::Storm,               1,          20,          35, RESIST_MAGIC | RESIST_FIRE | IMMUNE_LIGHTNING,  UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_FROSTC,   P_("monster", "Bluehorn"),                 "general",      11,     477, MonsterAIID::Rhino,               1,          25,          30, IMMUNE_MAGIC | RESIST_FIRE,                     UniqueMonsterPack::Leashed,                0,                90, TEXT_NONE     },
{ MT_HELLBURN, P_("monster", "Warpfire Hellspawn"),       "general",      11,     525, MonsterAIID::FireMan,             3,          10,          40, RESIST_MAGIC | IMMUNE_FIRE,                     UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_NSNAKE,   P_("monster", "Fangspeir"),                "general",      11,     444, MonsterAIID::SkeletonMelee,       1,          15,          32, IMMUNE_FIRE,                                    UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_UDEDBLRG, P_("monster", "Festerskull"),              "general",      11,     600, MonsterAIID::Storm,               2,          15,          30, IMMUNE_MAGIC,                                   UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_NBLACK,   P_("monster", "Lionskull the Bent"),       "general",      12,     525, MonsterAIID::SkeletonMelee,       2,          25,          25, IMMUNE_MAGIC | IMMUNE_FIRE | IMMUNE_LIGHTNING,  UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_COUNSLR,  P_("monster", "Blacktongue"),              "general",      12,     360, MonsterAIID::Counselor,           3,          15,          30, RESIST_FIRE,                                    UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_DEATHW,   P_("monster", "Viletouch"),                "general",      12,     525, MonsterAIID::Gargoyle,            3,          20,          40, IMMUNE_LIGHTNING,                               UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_RSNAKE,   P_("monster", "Viperflame"),               "general",      12,     570, MonsterAIID::SkeletonMelee,       1,          25,          35, IMMUNE_FIRE | RESIST_LIGHTNING,                 UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_BSNAKE,   P_("monster", "Fangskin"),                 "bhka",         14,     681, MonsterAIID::SkeletonMelee,       2,          15,          50, IMMUNE_MAGIC |               RESIST_LIGHTNING,  UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_SUCCUBUS, P_("monster", "Witchfire the Unholy"),     "general",      12,     444, MonsterAIID::Succubus,            3,          10,          20, IMMUNE_MAGIC | IMMUNE_FIRE | RESIST_LIGHTNING,  UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_BALROG,   P_("monster", "Blackskull"),               "bhka",         13,     750, MonsterAIID::SkeletonMelee,       3,          25,          40, IMMUNE_MAGIC |               RESIST_LIGHTNING,  UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_UNRAV,    P_("monster", "Soulslash"),                "general",      12,     450, MonsterAIID::SkeletonMelee,       0,          25,          25, IMMUNE_MAGIC,                                   UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_VTEXLRD,  P_("monster", "Windspawn"),                "general",      12,     711, MonsterAIID::SkeletonMelee,       1,          35,          40, IMMUNE_MAGIC | IMMUNE_FIRE,                     UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_GSNAKE,   P_("monster", "Lord of the Pit"),          "general",      13,     762, MonsterAIID::SkeletonMelee,       2,          25,          42, RESIST_FIRE,                                    UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_RTBLACK,  P_("monster", "Rustweaver"),               "general",      13,     400, MonsterAIID::SkeletonMelee,       3,           1,          60, IMMUNE_MAGIC | IMMUNE_FIRE | IMMUNE_LIGHTNING,  UniqueMonsterPack::None,                   0,                 0, TEXT_NONE     },
{ MT_HOLOWONE, P_("monster", "Howlingire the Shade"),     "general",      13,     450, MonsterAIID::SkeletonMelee,       2,          40,          75, RESIST_FIRE | RESIST_LIGHTNING,                 UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_MAEL,     P_("monster", "Doomcloud"),                "general",      13,     612, MonsterAIID::Storm,               1,           1,          60, RESIST_FIRE | IMMUNE_LIGHTNING,                 UniqueMonsterPack::None,                   0,                 0, TEXT_NONE     },
{ MT_PAINMSTR, P_("monster", "Bloodmoon Soulfire"),       "general",      13,     684, MonsterAIID::SkeletonMelee,       1,          15,          40, IMMUNE_MAGIC | RESIST_FIRE | RESIST_LIGHTNING,  UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_SNOWWICH, P_("monster", "Witchmoon"),                "general",      13,     310, MonsterAIID::Succubus,            3,          30,          40, RESIST_LIGHTNING,                               UniqueMonsterPack::None,                   0,                 0, TEXT_NONE     },
{ MT_VTEXLRD,  P_("monster", "Gorefeast"),                "general",      13,     771, MonsterAIID::SkeletonMelee,       3,          20,          55, RESIST_FIRE,                                    UniqueMonsterPack::None,                   0,                 0, TEXT_NONE     },
{ MT_RTBLACK,  P_("monster", "Graywar the Slayer"),       "general",      14,     672, MonsterAIID::SkeletonMelee,       1,          30,          50, RESIST_LIGHTNING,                               UniqueMonsterPack::None,                   0,                 0, TEXT_NONE     },
{ MT_MAGISTR,  P_("monster", "Dreadjudge"),               "general",      14,     540, MonsterAIID::Counselor,           1,          30,          40, IMMUNE_MAGIC | RESIST_FIRE | RESIST_LIGHTNING,  UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_HLSPWN,   P_("monster", "Stareye the Witch"),        "general",      14,     726, MonsterAIID::Succubus,            2,          30,          50, IMMUNE_FIRE,                                    UniqueMonsterPack::None,                   0,                 0, TEXT_NONE     },
{ MT_BTBLACK,  P_("monster", "Steelskull the Hunter"),    "general",      14,     831, MonsterAIID::SkeletonMelee,       3,          40,          50, RESIST_LIGHTNING,                               UniqueMonsterPack::None,                   0,                 0, TEXT_NONE     },
{ MT_RBLACK,   P_("monster", "Sir Gorash"),               "general",      16,    1050, MonsterAIID::SkeletonMelee,       1,          20,          60, 0,                                              UniqueMonsterPack::None,                   0,                 0, TEXT_NONE     },
{ MT_CABALIST, P_("monster", "The Vizier"),               "general",      15,     850, MonsterAIID::Counselor,           2,          25,          40, IMMUNE_FIRE,                                    UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_REALWEAV, P_("monster", "Zamphir"),                  "general",      15,     891, MonsterAIID::SkeletonMelee,       2,          30,          50, IMMUNE_MAGIC | RESIST_FIRE | RESIST_LIGHTNING,  UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_HLSPWN,   P_("monster", "Bloodlust"),                "general",      15,     825, MonsterAIID::Succubus,            1,          20,          55, IMMUNE_MAGIC |               IMMUNE_LIGHTNING,  UniqueMonsterPack::None,                   0,                 0, TEXT_NONE     },
{ MT_HLSPWN,   P_("monster", "Webwidow"),                 "general",      16,     774, MonsterAIID::Succubus,            1,          20,          50, IMMUNE_MAGIC | IMMUNE_FIRE,                     UniqueMonsterPack::None,                   0,                 0, TEXT_NONE     },
{ MT_SOLBRNR,  P_("monster", "Fleshdancer"),              "general",      16,     999, MonsterAIID::Succubus,            3,          30,          50, IMMUNE_MAGIC | RESIST_FIRE,                     UniqueMonsterPack::None,                   0,                 0, TEXT_NONE     },
{ MT_OBLORD,   P_("monster", "Grimspike"),                "general",      19,     534, MonsterAIID::Sneak,               1,          25,          40, IMMUNE_MAGIC | RESIST_FIRE,                     UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
// TRANSLATORS: Unique Monster Block end
{ MT_STORML,   P_("monster", "Doomlock"),                 "general",      28,     534, MonsterAIID::Sneak,               1,          35,          55, IMMUNE_MAGIC | RESIST_FIRE | RESIST_LIGHTNING,  UniqueMonsterPack::Leashed,                0,                 0, TEXT_NONE     },
{ MT_INVALID,  nullptr,                                   nullptr,         0,       0, MonsterAIID::Invalid,             0,           0,           0, 0,                                              UniqueMonsterPack::None,                   0,                 0, TEXT_NONE     },
	// clang-format on
};

} // namespace devilution
