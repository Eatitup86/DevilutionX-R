#include "DiabloUI/button.h"

#include "DiabloUI/diabloui.h"
#include "engine/load_pcx.hpp"
#include "engine/pcx_sprite.hpp"
#include "engine/render/pcx_render.hpp"
#include "engine/render/text_render.hpp"
#include "utils/display.h"

namespace devilution {

namespace {

std::optional<OwnedPcxSpriteSheet> ButtonSprites;

} // namespace

void LoadDialogButtonGraphics()
{
	ButtonSprites = LoadPcxSpriteSheetAsset("ui_art\\dvl_but_sml.pcx", 2);
	if (ButtonSprites == std::nullopt) {
		ButtonSprites = LoadPcxSpriteSheetAsset("ui_art\\but_sml.pcx", 15);
	}
}

void FreeDialogButtonGraphics()
{
	ButtonSprites = std::nullopt;
}

PcxSprite ButtonSprite(bool pressed)
{
	return PcxSpriteSheet { *ButtonSprites }.sprite(pressed ? 1 : 0);
}

void RenderButton(UiButton *button)
{
	const Surface &out = Surface(DiabloUiSurface()).subregion(button->m_rect.x, button->m_rect.y, button->m_rect.w, button->m_rect.h);
	RenderPcxSprite(out, ButtonSprite(button->IsPressed()), { 0, 0 });

	Rectangle textRect { { 0, 0 }, { button->m_rect.w, button->m_rect.h } };
	if (!button->IsPressed()) {
		--textRect.position.y;
	}

	DrawString(out, button->GetText(), textRect, UiFlags::AlignCenter | UiFlags::FontSizeDialog | UiFlags::ColorDialogWhite);
}

bool HandleMouseEventButton(const SDL_Event &event, UiButton *button)
{
	if (event.button.button != SDL_BUTTON_LEFT)
		return false;
	switch (event.type) {
	case SDL_MOUSEBUTTONUP:
		if (button->IsPressed()) {
			button->Activate();
			return true;
		}
		return false;
	case SDL_MOUSEBUTTONDOWN:
		button->Press();
		return true;
	default:
		return false;
	}
}

void HandleGlobalMouseUpButton(UiButton *button)
{
	button->Release();
}

} // namespace devilution
