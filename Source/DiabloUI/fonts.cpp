#include "DiabloUI/fonts.h"

#include "diablo.h"
#include "utils/file_util.h"
#include "utils/paths.h"
#include "utils/log.hpp"

namespace devilution {

TTF_Font *font = nullptr;
BYTE *FontTables[4];
Art ArtFonts[4][2];
/** This is so we know ttf has been init when we get to the diablo_deinit() function */
bool was_fonts_init = false;

namespace {

void LoadArtFont(const char *pszFile, int size, int color)
{
	LoadMaskedArt(pszFile, &ArtFonts[size][color], 256, 32);
}

} // namespace

void LoadArtFonts()
{
	FontTables[AFT_SMALL] = LoadFileInMem("ui_art\\font16.bin", nullptr);
	FontTables[AFT_MED] = LoadFileInMem("ui_art\\font24.bin", nullptr);
	FontTables[AFT_BIG] = LoadFileInMem("ui_art\\font30.bin", nullptr);
	FontTables[AFT_HUGE] = LoadFileInMem("ui_art\\font42.bin", nullptr);
	LoadArtFont("ui_art\\font16s.pcx", AFT_SMALL, AFC_SILVER);
	LoadArtFont("ui_art\\font16g.pcx", AFT_SMALL, AFC_GOLD);
	LoadArtFont("ui_art\\font24s.pcx", AFT_MED, AFC_SILVER);
	LoadArtFont("ui_art\\font24g.pcx", AFT_MED, AFC_GOLD);
	LoadArtFont("ui_art\\font30s.pcx", AFT_BIG, AFC_SILVER);
	LoadArtFont("ui_art\\font30g.pcx", AFT_BIG, AFC_GOLD);
	LoadArtFont("ui_art\\font42g.pcx", AFT_HUGE, AFC_GOLD);
}

void UnloadArtFonts()
{
	ArtFonts[AFT_SMALL][AFC_SILVER].Unload();
	ArtFonts[AFT_SMALL][AFC_GOLD].Unload();
	ArtFonts[AFT_MED][AFC_SILVER].Unload();
	ArtFonts[AFT_MED][AFC_GOLD].Unload();
	ArtFonts[AFT_BIG][AFC_SILVER].Unload();
	ArtFonts[AFT_BIG][AFC_GOLD].Unload();
	ArtFonts[AFT_HUGE][AFC_GOLD].Unload();
	mem_free_dbg(FontTables[AFT_SMALL]);
	FontTables[AFT_SMALL] = nullptr;
	mem_free_dbg(FontTables[AFT_MED]);
	FontTables[AFT_MED] = nullptr;
	mem_free_dbg(FontTables[AFT_BIG]);
	FontTables[AFT_BIG] = nullptr;
	mem_free_dbg(FontTables[AFT_HUGE]);
	FontTables[AFT_HUGE] = nullptr;
}

void LoadTtfFont()
{
	if (TTF_WasInit() == 0) {
		if (TTF_Init() == -1) {
			Log("TTF_Init: {}", TTF_GetError());
			diablo_quit(1);
		}
		was_fonts_init = true;
	}

	std::string ttfFontPath = GetTtfPath() + GetTtfName();
#ifdef __linux__
	if (!FileExists(ttfFontPath.c_str())) {
		ttfFontPath = "/usr/share/fonts/truetype/" + GetTtfName();
	}
#endif
	font = TTF_OpenFont(ttfFontPath.c_str(), 17);
	if (font == nullptr) {
		Log("TTF_OpenFont: {}", TTF_GetError());
		return;
	}

	TTF_SetFontKerning(font, 0);
	TTF_SetFontHinting(font, TTF_HINTING_MONO);
}

void UnloadTtfFont()
{
	if (font != nullptr && TTF_WasInit() != 0)
		TTF_CloseFont(font);
	font = nullptr;
}

void FontsCleanup()
{
	TTF_Quit();
}

} // namespace devilution
