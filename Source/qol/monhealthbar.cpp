/**
* @file monhealthbar.cpp
*
* Adds monster health bar QoL feature
*/

#include "DiabloUI/art_draw.h"
#include "control.h"
#include "cursor.h"
#include "options.h"
#include "qol/common.h"
#include "utils/language.h"

namespace devilution {
namespace {

Art healthBox;
Art resistance;
Art health;

} // namespace

void InitMonsterHealthBar()
{
	if (!sgOptions.Gameplay.bEnemyHealthBar)
		return;

	LoadMaskedArt("data\\healthbox.pcx", &healthBox, 1, 1);
	LoadArt("data\\health.pcx", &health);
	LoadMaskedArt("data\\resistance.pcx", &resistance, 6, 1);

	if ((healthBox.surface == nullptr)
	    || (health.surface == nullptr)
	    || (resistance.surface == nullptr)) {
		app_fatal(_("Failed to load UI resources. Is devilutionx.mpq accessible and up to date?"));
	}
}

void FreeMonsterHealthBar()
{
	healthBox.Unload();
	health.Unload();
	resistance.Unload();
}

void DrawMonsterHealthBar(const CelOutputBuffer &out)
{
	if (!sgOptions.Gameplay.bEnemyHealthBar)
		return;

	assert(healthBox.surface != nullptr);
	assert(health.surface != nullptr);
	assert(resistance.surface != nullptr);

	if (currlevel == 0)
		return;
	if (pcursmonst == -1)
		return;

	const MonsterStruct &mon = monster[pcursmonst];

	const int width = healthBox.w();
	const int height = healthBox.h();
	int xPos = (gnScreenWidth - width) / 2;

	if (PANELS_COVER) {
		if (invflag || sbookflag)
			xPos -= SPANEL_WIDTH / 2;
		if (chrflag || questlog)
			xPos += SPANEL_WIDTH / 2;
	}

	const int yPos = 18;
	const int border = 3;

	const int maxLife = std::max(mon._mmaxhp, mon._mhitpoints);

	DrawArt(out, xPos, yPos, &healthBox);
	DrawHalfTransparentRectTo(out, xPos + border, yPos + border, width - (border * 2), height - (border * 2));
	int barProgress = (width * mon._mhitpoints) / maxLife;
	if (barProgress) {
		DrawArt(out, xPos + border + 1, yPos + border + 1, &health, 0, barProgress, height - (border * 2) - 2);
	}

	if (sgOptions.Gameplay.bShowMonsterType) {
		Uint8 borderColors[] = { 248 /*undead*/, 232 /*demon*/, 150 /*beast*/ };
		Uint8 borderColor = borderColors[mon.MData->mMonstClass];
		int borderWidth = width - (border * 2);
		FastDrawHorizLine(out, xPos + border, yPos + border, borderWidth, borderColor);
		FastDrawHorizLine(out, xPos + border, yPos + height - border - 1, borderWidth, borderColor);
		int borderHeight = height - (border * 2) - 2;
		FastDrawVertLine(out, xPos + border, yPos + border + 1, borderHeight, borderColor);
		FastDrawVertLine(out, xPos + width - border - 1, yPos + border + 1, borderHeight, borderColor);
	}

	int barLableX = xPos + width / 2 - GetTextWidth(mon.mName) / 2;
	int barLableY = yPos + 10 + (height - 11) / 2;
	PrintGameStr(out, barLableX - 1, barLableY + 1, mon.mName, COL_BLACK);
	text_color color = COL_WHITE;
	if (mon._uniqtype != 0)
		color = COL_GOLD;
	else if (mon.leader != 0)
		color = COL_BLUE;
	PrintGameStr(out, barLableX, barLableY, mon.mName, color);

	if (mon._uniqtype != 0 || monstkills[mon.MType->mtype] >= 15) {
		monster_resistance immunes[] = { IMMUNE_MAGIC, IMMUNE_FIRE, IMMUNE_LIGHTNING };
		monster_resistance resists[] = { RESIST_MAGIC, RESIST_FIRE, RESIST_LIGHTNING };

		int resOffset = 5;
		for (int i = 0; i < 3; i++) {
			if (mon.mMagicRes & immunes[i]) {
				DrawArt(out, xPos + resOffset, yPos + height - 6, &resistance, i * 2 + 1);
				resOffset += resistance.w() + 2;
			} else if (mon.mMagicRes & resists[i]) {
				DrawArt(out, xPos + resOffset, yPos + height - 6, &resistance, i * 2);
				resOffset += resistance.w() + 2;
			}
		}
	}
}

} // namespace devilution
