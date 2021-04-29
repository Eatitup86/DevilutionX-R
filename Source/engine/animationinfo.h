/**
 * @file animationinfo.h
 *
 * Contains the core animation information and related logic
 */
#pragma once

#include <stdint.h>

namespace devilution {

/**
 * @brief Specifies what special logics are applied for a Animation
 */
enum class AnimationDistributionParams : uint8_t {
	None,
	/*
	* @brief ProcessAnimation will be called after SetNewAnimation (in same GameTick as NewPlrAnim)
	*/
	ProcessAnimationPending,
	/*
	* @brief Delay of last Frame is ignored (for example, because only Frame and not delay is checked in game_logic)
	*/
	SkipsDelayOfLastFrame,
};

/*
* @brief Contains the core animation information and related logic
*/
class AnimationInfo {
public:
	/*
	* @brief Pointer to Animation Data
	*/
	uint8_t *pData;
	/*
	* @brief Additional delay of each animation in the current animation
	*/
	int DelayLen;
	/*
	* @brief Increases by one each game tick, counting how close we are to DelayLen
	*/
	int DelayCounter;
	/*
	* @brief Number of frames in current animation
	*/
	int NumberOfFrames;
	/*
	* @brief Current frame of animation
	*/
	int CurrentFrame;

	/**
	 * @brief Calculates the Frame to use for the Animation rendering
	 * @return The Frame to use for rendering
	 */
	int GetFrameToUseForRendering();

	/**
	 * @brief Sets the new Animation with all relevant information for rendering
	 * @param pData Pointer to Animation Data
	 * @param numberOfFrames Number of Frames in Animation
	 * @param delayLen Delay after each Animation sequence
	 * @param params Specifies what special logics are applied to this Animation
	 * @param numSkippedFrames Number of Frames that will be skipped (for example with modifier "faster attack")
	 * @param distributeFramesBeforeFrame Distribute the numSkippedFrames only before this frame
	 */
	void SetNewAnimation(uint8_t *pData, int numberOfFrames, int delayLen, AnimationDistributionParams params = AnimationDistributionParams::None, int numSkippedFrames = 0, int distributeFramesBeforeFrame = 0);

	/*
	* @brief Process the Animation for a GameTick (for example advances the frame)
	*/
	void ProcessAnimation();

private:
	/*
	* @brief Specifies how many animations-fractions are displayed between two gameticks. this can be > 0, if animations are skipped or < 0 if the same animation is shown in multiple times (delay specified).
	*/
	float GameTickModifier;
	/*
	* @brief Number of GameTicks after the current animation sequence started
	*/
	int GameTicksSinceSequenceStarted;
	/*
	* @brief Animation Frames that will be adjusted for the skipped Frames/GameTicks
	*/
	int RelevantFramesForDistributing;
};

} // namespace devilution
