#include "utils/soundsample.h"

#include <chrono>
#include <cmath>
#include <utility>

#ifndef PS2
#include <Aulib/DecoderDrmp3.h>
#include <Aulib/DecoderDrwav.h>
#endif
#include <SDL.h>
#ifdef USE_SDL1
#include "utils/sdl2_to_1_2_backports.h"
#else
#include "utils/sdl2_backports.h"
#endif

#include "engine/assets.hpp"
#include "options.h"
#ifndef PS2
#include "utils/aulib.hpp"
#endif
#include "utils/log.hpp"
#include "utils/math.h"
#include "utils/stubs.h"

namespace devilution {

namespace {

constexpr float LogBase = 10.0;

/**
 * Scaling factor for attenuating volume.
 * Picked so that a volume change of -10 dB results in half perceived loudness.
 * VolumeScale = -1000 / log(0.5)
 */
constexpr float VolumeScale = 3321.9281;

/**
 * Min and max volume range, in millibel.
 * -100 dB (muted) to 0 dB (max. loudness).
 */
constexpr float MillibelMin = -10000.F;
constexpr float MillibelMax = 0.F;

/**
 * Stereo separation factor for left/right speaker panning. Lower values increase separation, moving
 * sounds further left/right, while higher values will pull sounds more towards the middle, reducing separation.
 * Current value is tuned to have ~2:1 mix for sounds that happen on the edge of a 640x480 screen.
 */
constexpr float StereoSeparation = 6000.F;

float PanLogToLinear(int logPan)
{
	if (logPan == 0)
		return 0;

	auto factor = std::pow(LogBase, static_cast<float>(-std::abs(logPan)) / StereoSeparation);

	return copysign(1.F - factor, static_cast<float>(logPan));
}

#ifndef PS2
std::unique_ptr<Aulib::Decoder> CreateDecoder(bool isMp3)
{
	if (isMp3)
		return std::make_unique<Aulib::DecoderDrmp3>();
	return std::make_unique<Aulib::DecoderDrwav>();
}

std::unique_ptr<Aulib::Stream> CreateStream(SDL_RWops *handle, bool isMp3)
{
	auto decoder = CreateDecoder(isMp3);
	if (!decoder->open(handle)) // open for `getRate`
		return nullptr;
	auto resampler = CreateAulibResampler(decoder->getRate());
	return std::make_unique<Aulib::Stream>(handle, std::move(decoder), std::move(resampler), /*closeRw=*/true);
}
#endif

/**
 * @brief Converts log volume passed in into linear volume.
 * @param logVolume Logarithmic volume in the range [logMin..logMax]
 * @param logMin Volume range minimum (usually ATTENUATION_MIN for game sounds and VOLUME_MIN for volume sliders)
 * @param logMax Volume range maximum (usually 0)
 * @return Linear volume in the range [0..1]
 */
float VolumeLogToLinear(int logVolume, int logMin, int logMax)
{
	const auto logScaled = math::Remap(static_cast<float>(logMin), static_cast<float>(logMax), MillibelMin, MillibelMax, static_cast<float>(logVolume));
	return std::pow(LogBase, logScaled / VolumeScale); // linVolume
}

} // namespace

///// SoundSample /////

void SoundSample::Release()
{
#ifndef PS2
	file_data_ = nullptr;
	file_data_size_ = 0;
#else
	if (stream_ != nullptr) // We are the owner
		audsrv_free_adpcm(sampleId_);
	sampleId_ = nullptr;
#endif
	stream_ = nullptr;
}

/**
 * @brief Check if a the sound is being played atm
 */
bool SoundSample::IsPlaying()
{
#ifndef PS2
	return stream_ && stream_->isPlaying();
#else
	if (channel_ == -1)
		return false;
	return audsrv_is_adpcm_playing(channel_, sampleId_) != 0;
#endif
}

bool SoundSample::Play(int numIterations)
{
#ifndef PS2
	if (!stream_->play(numIterations)) {
		LogError(LogCategory::Audio, "Aulib::Stream::play (from SoundSample::Play): {}", SDL_GetError());
		return false;
	}
#else
	if (IsStreaming()) {
		return true;
	}

	int channel = audsrv_ch_play_adpcm(-1, sampleId_);
	printf("channel %d\n", channel);
	if (channel < 0) {
		LogError(LogCategory::Audio, "audsrv_ch_play_adpcm (from SoundSample::Play): {}", channel);
		return false;
	}
	channel_ = channel;

	audsrv_adpcm_set_volume_and_pan(channel_, volume_, pan_);
#endif
	return true;
}

int SoundSample::SetChunkStream(std::string filePath, bool isMp3, bool logErrors)
{
	SDL_RWops *handle = OpenAssetAsSdlRwOps(filePath.c_str(), /*threadsafe=*/true);
	if (handle == nullptr) {
		if (logErrors)
			LogError(LogCategory::Audio, "OpenAsset failed (from SoundSample::SetChunkStream) for {}: {}", filePath, SDL_GetError());
		return -1;
	}
	file_path_ = std::move(filePath);
#ifndef PS2
	isMp3_ = isMp3;
	stream_ = CreateStream(handle, isMp3);
	if (!stream_->open()) {
		stream_ = nullptr;
		if (logErrors)
			LogError(LogCategory::Audio, "Aulib::Stream::open (from SoundSample::SetChunkStream) for {}: {}", file_path_, SDL_GetError());
		return -1;
	}
#endif
	return 0;
}

int SoundSample::SetChunk(ArraySharedPtr<std::uint8_t> fileData, std::size_t dwBytes, bool isMp3)
{
#ifndef PS2
	isMp3_ = isMp3;
	file_data_ = std::move(fileData);
	file_data_size_ = dwBytes;
	SDL_RWops *buf = SDL_RWFromConstMem(file_data_.get(), dwBytes);
	if (buf == nullptr) {
		return -1;
	}

	stream_ = CreateStream(buf, isMp3_);
	if (!stream_->open()) {
		stream_ = nullptr;
		file_data_ = nullptr;
		LogError(LogCategory::Audio, "Aulib::Stream::open (from SoundSample::SetChunk): {}", SDL_GetError());
		return -1;
	}
#else
	stream_ = std::make_unique<audsrv_adpcm_t>();
	int success = audsrv_load_adpcm(stream_.get(), fileData.get(), dwBytes);
	if (success != 0) {
		LogError(LogCategory::Audio, "audsrv_load_adpcm (from SoundSample::SetChunk): {}", success);
		return -1;
	}
	sampleId_ = stream_.get();
#endif
	return 0;
}

void SoundSample::SetVolume(int logVolume, int logMin, int logMax)
{
#ifndef PS2
	stream_->setVolume(VolumeLogToLinear(logVolume, logMin, logMax));
#else
	volume_ = VolumeLogToLinear(logVolume, logMin, logMax) * MAX_VOLUME;
	if (channel_ == -1)
		return;

	audsrv_adpcm_set_volume_and_pan(channel_, volume_, pan_);
#endif
}

void SoundSample::SetStereoPosition(int logPan)
{
#ifndef PS2
	stream_->setStereoPosition(PanLogToLinear(logPan));
#else
	pan_ = PanLogToLinear(logPan) * 100;
	if (channel_ == -1)
		return;

	audsrv_adpcm_set_volume_and_pan(channel_, volume_, pan_);
#endif
}

int SoundSample::GetLength() const
{
#ifndef PS2
	if (!stream_)
		return 0;
	return std::chrono::duration_cast<std::chrono::milliseconds>(stream_->duration()).count();
#else
	size_t size = 0;
	int pitch = 0;

	if (stream_ != nullptr) {
		size = stream_->size;
		pitch = stream_->pitch;
	} else if (!file_path_.empty()) {
		AssetHandle handle = OpenAsset(file_path_.c_str(), size);
		if (handle.ok()) {
			size -= 16;
			uint32_t buffer[3];
			if (handle.read(buffer, sizeof(buffer))) {
				pitch = buffer[2];
			}
		}
	}

	if (pitch == 0)
		return 0;

	uint64_t microSamples = size;
	microSamples *= 56 * 1000;

	return microSamples / (pitch * 375);
#endif
}

} // namespace devilution
