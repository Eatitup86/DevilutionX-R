#include "DiabloUI/diabloui.h"
#include "DiabloUI/text.h"
#include "stores.h"
#include "storm/storm.h"
#include "utils/language.h"

namespace devilution {

int provider;
namespace {

char selconn_MaxPlayers[21];
char selconn_Description[64];
char selconn_Gateway[129];
bool selconn_ReturnValue = false;
bool selconn_EndMenu = false;
GameData *selconn_GameData;

std::vector<UiListItem *> vecConnItems;
std::vector<UiItemBase *> vecSelConnDlg;

#define DESCRIPTION_WIDTH 205

void SelconnEsc();
void SelconnFocus(int value);
void SelconnSelect(int value);

void SelconnLoad()
{
	LoadBackgroundArt("ui_art\\selconn.pcx");

#ifndef NONET
#ifndef DISABLE_ZERO_TIER
	vecConnItems.push_back(new UiListItem("Zerotier", SELCONN_ZT));
#endif
#ifndef DISABLE_TCP
	vecConnItems.push_back(new UiListItem(_("Client-Server (TCP)"), SELCONN_TCP));
#endif
#endif
	vecConnItems.push_back(new UiListItem(_("Loopback"), SELCONN_LOOPBACK));

	UiAddBackground(&vecSelConnDlg);
	UiAddLogo(&vecSelConnDlg);

	SDL_Rect rect1 = { (Sint16)(PANEL_LEFT + 24), (Sint16)(Sint16)(UI_OFFSET_Y + 161), 590, 35 };
	vecSelConnDlg.push_back(new UiArtText(_("Multi Player Game"), rect1, UIS_CENTER | UIS_BIG));

	SDL_Rect rect2 = { (Sint16)(PANEL_LEFT + 35), (Sint16)(UI_OFFSET_Y + 218), DESCRIPTION_WIDTH, 21 };
	vecSelConnDlg.push_back(new UiArtText(selconn_MaxPlayers, rect2));

	SDL_Rect rect3 = { (Sint16)(PANEL_LEFT + 35), (Sint16)(UI_OFFSET_Y + 256), DESCRIPTION_WIDTH, 21 };
	vecSelConnDlg.push_back(new UiArtText(_("Requirements:"), rect3));

	SDL_Rect rect4 = { (Sint16)(PANEL_LEFT + 35), (Sint16)(UI_OFFSET_Y + 275), DESCRIPTION_WIDTH, 66 };
	vecSelConnDlg.push_back(new UiArtText(selconn_Description, rect4));

	SDL_Rect rect5 = { (Sint16)(PANEL_LEFT + 30), (Sint16)(UI_OFFSET_Y + 356), 220, 31 };
	vecSelConnDlg.push_back(new UiArtText(_("no gateway needed"), rect5, UIS_CENTER | UIS_MED));

	SDL_Rect rect6 = { (Sint16)(PANEL_LEFT + 35), (Sint16)(UI_OFFSET_Y + 393), DESCRIPTION_WIDTH, 21 };
	vecSelConnDlg.push_back(new UiArtText(selconn_Gateway, rect6, UIS_CENTER));

	SDL_Rect rect7 = { (Sint16)(PANEL_LEFT + 300), (Sint16)(UI_OFFSET_Y + 211), 295, 33 };
	vecSelConnDlg.push_back(new UiArtText(_("Select Connection"), rect7, UIS_CENTER | UIS_BIG));

	SDL_Rect rect8 = { (Sint16)(PANEL_LEFT + 16), (Sint16)(UI_OFFSET_Y + 427), 250, 35 };
	vecSelConnDlg.push_back(new UiArtTextButton(_("Change Gateway"), nullptr, rect8, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD | UIS_HIDDEN));

	vecSelConnDlg.push_back(new UiList(vecConnItems, PANEL_LEFT + 305, (UI_OFFSET_Y + 256), 285, 26, UIS_CENTER | UIS_VCENTER | UIS_GOLD));

	SDL_Rect rect9 = { (Sint16)(PANEL_LEFT + 299), (Sint16)(UI_OFFSET_Y + 427), 140, 35 };
	vecSelConnDlg.push_back(new UiArtTextButton(_("OK"), &UiFocusNavigationSelect, rect9, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD));

	SDL_Rect rect10 = { (Sint16)(PANEL_LEFT + 454), (Sint16)(UI_OFFSET_Y + 427), 140, 35 };
	vecSelConnDlg.push_back(new UiArtTextButton(_("Cancel"), &UiFocusNavigationEsc, rect10, UIS_CENTER | UIS_VCENTER | UIS_BIG | UIS_GOLD));

	UiInitList(vecConnItems.size(), SelconnFocus, SelconnSelect, SelconnEsc, vecSelConnDlg);
}

void SelconnFree()
{
	ArtBackground.Unload();

	for (auto pUIItem : vecConnItems) {
		delete pUIItem;
	}
	vecConnItems.clear();

	for (auto pUIMenuItem : vecSelConnDlg) {
		delete pUIMenuItem;
	}
	vecSelConnDlg.clear();
}

void SelconnEsc()
{
	selconn_ReturnValue = false;
	selconn_EndMenu = true;
}

void SelconnFocus(int value)
{
	int players = MAX_PLRS;
	switch (vecConnItems[value]->m_value) {
	case SELCONN_TCP:
		strncpy(selconn_Description, _("All computers must be connected to a TCP-compatible network."), sizeof(selconn_Description) - 1);
		players = MAX_PLRS;
		break;
	case SELCONN_ZT:
		strncpy(selconn_Description, _("All computers must be connected to the internet."), sizeof(selconn_Description) - 1);
		players = MAX_PLRS;
		break;
	case SELCONN_LOOPBACK:
		strncpy(selconn_Description, _("Play by yourself with no network exposure."), sizeof(selconn_Description) - 1);
		players = 1;
		break;
	}

	snprintf(selconn_MaxPlayers, sizeof(selconn_MaxPlayers), _("Players Supported: %i"), players);
	WordWrapArtStr(selconn_Description, DESCRIPTION_WIDTH);
}

void SelconnSelect(int value)
{
	provider = vecConnItems[value]->m_value;

	SelconnFree();
	selconn_EndMenu = SNetInitializeProvider(provider, selconn_GameData);
	SelconnLoad();
}

} // namespace

bool UiSelectProvider(GameData *gameData)
{
	selconn_GameData = gameData;
	SelconnLoad();

	selconn_ReturnValue = true;
	selconn_EndMenu = false;
	while (!selconn_EndMenu) {
		UiClearScreen();
		UiPollAndRender();
	}
	SelconnFree();

	return selconn_ReturnValue;
}

} // namespace devilution
