// Stubbed implementations of sound functions for the NOSOUND mode.
#include "sound.h"

namespace devilution {

bool gbSndInited;
bool gbMusicOn;
bool gbSoundOn;

// Disable clang-format here because our config says:
// AllowShortFunctionsOnASingleLine: None
// clang-format off
void snd_update(bool bStopAll) { }
void snd_stop_snd(TSnd *pSnd) { }
bool snd_playing(TSnd *pSnd) { return false; }
void snd_play_snd(TSnd *pSnd, int lVolume, int lPan) { }
TSnd *sound_file_load(const char *path, bool stream) { return nullptr; }
void sound_file_cleanup(TSnd *sound_file) { }
void snd_init() { }
void snd_deinit() { }
void music_stop() { }
void music_start(uint8_t nTrack) { }
void sound_disable_music(bool disable) { }
int sound_get_or_set_music_volume(int volume) { return 0; }
int sound_get_or_set_sound_volume(int volume) { return 0; }
// clang-format on

} // namespace devilution
