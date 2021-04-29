#include <gtest/gtest.h>

#include "nthread.h"
#include "engine/animationinfo.h"

using namespace devilution;

/**
 * @brief Represents a Action in the game_logic or rendering.
 */
struct TestData {
	virtual ~TestData() = default;
};

/**
 * @brief Represents a GameTick, this includes skipping of Frames (for example because of Fastest Attack Modifier) and ProcessAnimation (which also updates Animation Frame/Count).
 */
struct GameTickData : TestData {
	int _ExpectedAnimationFrame;
	int _ExpectedAnimationCnt;
	int _FramesToSkip;
	GameTickData(int expectedAnimationFrame, int expectedAnimationCnt, int framesToSkip = 0)
	{
		_ExpectedAnimationFrame = expectedAnimationFrame;
		_ExpectedAnimationCnt = expectedAnimationCnt;
		_FramesToSkip = framesToSkip;
	}
};

/**
 * @brief Represents a rendering/drawing Action.
 * This happens directly after the game_logic (_fProgressToNextGameTick = 0) or if no GameTick is due between two GameTicks (_fProgressToNextGameTick != 0).
 */
struct RenderingData : TestData {
	/*
	* @brief the progress as a fraction (0.0f to 1.0f) in time to the next game tick
	*/
	float _fProgressToNextGameTick;
	int _ExpectedRenderingFrame;
	RenderingData(float fProgressToNextGameTick, int expectedRenderingFrame)
	{
		this->_fProgressToNextGameTick = fProgressToNextGameTick;
		this->_ExpectedRenderingFrame = expectedRenderingFrame;
	}
};

/*
* @brief
* This UnitTest tries to simulate the GameLoop (GameTickData) and the Rendering that can happen (RenderingData).
* Rendering can happen more often then GameTicks and at any time between two GameTicks.
* The Animation Distribution Logic must adjust to the Rendering that happens at any give point in time.
*/
void RunAnimationTest(int numFrames, int delay, AnimationDistributionParams params, int numSkippedFrames, int distributeFramesBeforeFrame, const std::vector<TestData *> &vecTestData)
{
	const int pnum = 0;
	AnimationInfo animInfo;
	animInfo.SetNewAnimation(nullptr, numFrames, delay, params, numSkippedFrames, distributeFramesBeforeFrame);

	int currentGameTick = 0;
	for (TestData *x : vecTestData) {
		auto gameTickData = dynamic_cast<GameTickData *>(x);
		if (gameTickData != nullptr) {
			currentGameTick += 1;
			if (gameTickData->_FramesToSkip != 0)
				pPlayer->AnimInfo.CurrentFrame += gameTickData->_FramesToSkip;
			ProcessPlayerAnimation(pnum);
			EXPECT_EQ(pPlayer->AnimInfo.CurrentFrame, gameTickData->_ExpectedAnimationFrame);
			EXPECT_EQ(pPlayer->AnimInfo.DelayCounter, gameTickData->_ExpectedAnimationCnt);
		}

		auto renderingData = dynamic_cast<RenderingData *>(x);
		if (renderingData != nullptr) {
			gfProgressToNextGameTick = renderingData->_fProgressToNextGameTick;
			EXPECT_EQ(GetFrameToUseForPlayerRendering(pPlayer), renderingData->_ExpectedRenderingFrame)
			    << std::fixed << std::setprecision(2)
			    << "ProgressToNextGameTick: " << renderingData->_fProgressToNextGameTick
			    << " CurrentFrame: " << pPlayer->AnimInfo.CurrentFrame
			    << " DelayCounter: " << pPlayer->AnimInfo.DelayCounter
			    << " GameTick: " << currentGameTick;
		}
	}
	for (TestData *x : vecTestData) {
		delete x;
	}
}

TEST(AnimationInfo, AttackSwordWarrior) // ProcessAnimationPending should be considered by distribution logic
{
	RunAnimationTest(16, 0, AnimationDistributionParams::ProcessAnimationPending, 0, 9,
	    {
	        // ProcessAnimation directly after StartAttack (in same GameTick). So we don't see any rendering before.
	        new GameTickData(2, 0),
	        new RenderingData(0.0f, 1),
	        new RenderingData(0.3f, 1),
	        new RenderingData(0.6f, 1),
	        new RenderingData(0.8f, 1),
	        new GameTickData(3, 0),
	        new RenderingData(0.0f, 2),
	        new RenderingData(0.3f, 2),
	        new RenderingData(0.6f, 2),
	        new RenderingData(0.8f, 3),
	        new GameTickData(4, 0),
	        new RenderingData(0.0f, 3),
	        new RenderingData(0.3f, 3),
	        new RenderingData(0.6f, 3),
	        new RenderingData(0.8f, 4),
	        new GameTickData(5, 0),
	        new RenderingData(0.0f, 4),
	        new RenderingData(0.3f, 4),
	        new RenderingData(0.6f, 5),
	        new RenderingData(0.8f, 5),
	        new GameTickData(6, 0),
	        new RenderingData(0.0f, 5),
	        new RenderingData(0.3f, 5),
	        new RenderingData(0.6f, 6),
	        new RenderingData(0.8f, 6),
	        new GameTickData(7, 0),
	        new RenderingData(0.0f, 6),
	        new RenderingData(0.3f, 7),
	        new RenderingData(0.6f, 7),
	        new RenderingData(0.8f, 7),
	        new GameTickData(8, 0),
	        new RenderingData(0.0f, 7),
	        new RenderingData(0.3f, 8),
	        new RenderingData(0.6f, 8),
	        new RenderingData(0.8f, 8),

			// After this GameTick, the Animation Distribution Logic is disabled
	        new GameTickData(9, 0),
	        new RenderingData(0.1f, 9),
	        new GameTickData(10, 0),
	        new RenderingData(0.4f, 10),
	        new GameTickData(11, 0),
	        new RenderingData(0.4f, 11),
	        new GameTickData(12, 0),
	        new RenderingData(0.3f, 12),
	        new GameTickData(13, 0),
	        new RenderingData(0.0f, 13),
	        new GameTickData(14, 0),
	        new RenderingData(0.6f, 14),
	        new GameTickData(15, 0),
	        new RenderingData(0.6f, 15),
	        new GameTickData(16, 0),
	        new RenderingData(0.6f, 16),
	        // Animation stopped cause PM_DoAttack would stop the Animation "if (plr[pnum].AnimInfo.CurrentFrame == plr[pnum]._pAFrames) {"
	    });
}

TEST(AnimationInfo, AttackSwordWarriorWithFastestAttack) // Skipped frames and ProcessAnimationPending should be considered by distribution logic
{
	RunAnimationTest(16, 0, AnimationDistributionParams::ProcessAnimationPending, 2, 9,
	    {
	        // ProcessAnimation directly after StartAttack (in same GameTick). So we don't see any rendering before.
	        new GameTickData(2, 0),
	        new RenderingData(0.0f, 1),
	        new RenderingData(0.3f, 1),
	        new RenderingData(0.6f, 1),
	        new RenderingData(0.8f, 2),
	        new GameTickData(3, 0),
	        new RenderingData(0.0f, 2),
	        new RenderingData(0.3f, 3),
	        new RenderingData(0.6f, 3),
	        new RenderingData(0.8f, 3),
	        new GameTickData(4, 0),
	        new RenderingData(0.0f, 4),
	        new RenderingData(0.3f, 4),
	        new RenderingData(0.6f, 5),
	        new RenderingData(0.8f, 5),
	        new GameTickData(7, 0, 2),
	        new RenderingData(0.0f, 5),
	        new RenderingData(0.3f, 6),
	        new RenderingData(0.6f, 6),
	        new RenderingData(0.8f, 7),
	        new GameTickData(8, 0),
	        new RenderingData(0.0f, 7),
	        new RenderingData(0.3f, 7),
	        new RenderingData(0.6f, 8),
	        new RenderingData(0.8f, 8),

			// After this GameTick, the Animation Distribution Logic is disabled
	        new GameTickData(9, 0),
	        new RenderingData(0.1f, 9),
	        new GameTickData(10, 0),
	        new RenderingData(0.4f, 10),
	        new GameTickData(11, 0),
	        new RenderingData(0.4f, 11),
	        new GameTickData(12, 0),
	        new RenderingData(0.3f, 12),
	        new GameTickData(13, 0),
	        new RenderingData(0.0f, 13),
	        new GameTickData(14, 0),
	        new RenderingData(0.6f, 14),
	        new GameTickData(15, 0),
	        new RenderingData(0.6f, 15),
	        new GameTickData(16, 0),
	        new RenderingData(0.6f, 16),
	        // Animation stopped cause PM_DoAttack would stop the Animation "if (plr[pnum].AnimInfo.CurrentFrame == plr[pnum]._pAFrames) {"
	    });
}

TEST(AnimationInfo, BlockingWarriorNormal) // Ignored delay for last Frame should be considered by distribution logic
{
	RunAnimationTest(2, 2, AnimationDistributionParams::SkipsDelayOfLastFrame, 0, 0,
	    {
	        new RenderingData(0.0f, 1),
	        new RenderingData(0.3f, 1),
	        new RenderingData(0.6f, 1),
	        new RenderingData(0.8f, 1),
	        new GameTickData(1, 1),
	        new RenderingData(0.0f, 1),
	        new RenderingData(0.3f, 1),
	        new RenderingData(0.6f, 1),
	        new RenderingData(0.8f, 1),
	        new GameTickData(1, 2),
	        new RenderingData(0.0f, 2),
	        new RenderingData(0.3f, 2),
	        new RenderingData(0.6f, 2),
	        new RenderingData(0.8f, 2),
	        new GameTickData(2, 0),
	        new RenderingData(0.0f, 2),
	        new RenderingData(0.3f, 2),
	        new RenderingData(0.6f, 2),
	        new RenderingData(0.8f, 2),
	        // Animation stopped cause PM_DoBlock would stop the Animation "if (plr[pnum].AnimInfo.CurrentFrame >= plr[pnum]._pBFrames) {"
	    });
}

TEST(AnimationInfo, BlockingSorcererWithFastBlock) // Skipped frames and ignored delay for last Frame should be considered by distribution logic
{
	RunAnimationTest(6, 2, AnimationDistributionParams::SkipsDelayOfLastFrame, 4, 0,
	    {
	        new RenderingData(0.0f, 1),
	        new RenderingData(0.3f, 1),
	        new RenderingData(0.6f, 1),
	        new RenderingData(0.8f, 2),
	        new GameTickData(1, 1),
	        new RenderingData(0.0f, 2),
	        new RenderingData(0.3f, 2),
	        new RenderingData(0.6f, 3),
	        new RenderingData(0.8f, 3),
	        new GameTickData(1, 2),
	        new RenderingData(0.0f, 4),
	        new RenderingData(0.3f, 4),
	        new RenderingData(0.6f, 4),
	        new RenderingData(0.8f, 5),
	        new GameTickData(2, 0),
	        new RenderingData(0.0f, 5),
	        new RenderingData(0.3f, 5),
	        new RenderingData(0.6f, 6),
	        new RenderingData(0.8f, 6),
	        // Animation stopped cause PM_DoBlock would stop the Animation "if (plr[pnum].AnimInfo.CurrentFrame >= plr[pnum]._pBFrames) {"
	    });
}

TEST(AnimationInfo, HitRecoverySorcererZenMode) // Skipped frames and ignored delay for last Frame should be considered by distribution logic
{
	RunAnimationTest(8, 0, AnimationDistributionParams::None, 4, 0,
	    {
	        new RenderingData(0.0f, 1),
	        new RenderingData(0.3f, 1),
	        new RenderingData(0.6f, 2),
	        new RenderingData(0.8f, 2),
	        new GameTickData(3, 0, 1),
	        new RenderingData(0.0f, 3),
	        new RenderingData(0.3f, 3),
	        new RenderingData(0.6f, 4),
	        new RenderingData(0.8f, 4),
	        new GameTickData(7, 0, 3),
	        new RenderingData(0.0f, 5),
	        new RenderingData(0.3f, 5),
	        new RenderingData(0.6f, 6),
	        new RenderingData(0.8f, 6),
	        new GameTickData(8, 0),
	        new RenderingData(0.0f, 7),
	        new RenderingData(0.3f, 7),
	        new RenderingData(0.6f, 8),
	        new RenderingData(0.8f, 8),
	        // Animation stopped cause PM_DoGotHit would stop the Animation "if (plr[pnum].AnimInfo.CurrentFrame >= plr[pnum]._pHFrames) {"
	    });
}
TEST(AnimationInfo, Stand) // Distribution Logic shouldn't change anything here
{
	RunAnimationTest(10, 3, AnimationDistributionParams::None, 0, 0,
	    {
	        new RenderingData(0.1f, 1),
	        new GameTickData(1, 1),
	        new RenderingData(0.6f, 1),
	        new GameTickData(1, 2),
	        new RenderingData(0.6f, 1),
	        new GameTickData(1, 3),
	        new RenderingData(0.6f, 1),

	        new GameTickData(2, 0),
	        new RenderingData(0.6f, 2),
	        new GameTickData(2, 1),
	        new RenderingData(0.6f, 2),
	        new GameTickData(2, 2),
	        new RenderingData(0.6f, 2),
	        new GameTickData(2, 3),
	        new RenderingData(0.6f, 2),

	        new GameTickData(3, 0),
	        new RenderingData(0.6f, 3),
	        new GameTickData(3, 1),
	        new RenderingData(0.6f, 3),
	        new GameTickData(3, 2),
	        new RenderingData(0.6f, 3),
	        new GameTickData(3, 3),
	        new RenderingData(0.6f, 3),

	        new GameTickData(4, 0),
	        new RenderingData(0.6f, 4),
	        new GameTickData(4, 1),
	        new RenderingData(0.6f, 4),
	        new GameTickData(4, 2),
	        new RenderingData(0.6f, 4),
	        new GameTickData(4, 3),
	        new RenderingData(0.6f, 4),

	        new GameTickData(5, 0),
	        new RenderingData(0.6f, 5),
	        new GameTickData(5, 1),
	        new RenderingData(0.6f, 5),
	        new GameTickData(5, 2),
	        new RenderingData(0.6f, 5),
	        new GameTickData(5, 3),
	        new RenderingData(0.6f, 5),

	        new GameTickData(6, 0),
	        new RenderingData(0.6f, 6),
	        new GameTickData(6, 1),
	        new RenderingData(0.6f, 6),
	        new GameTickData(6, 2),
	        new RenderingData(0.6f, 6),
	        new GameTickData(6, 3),
	        new RenderingData(0.6f, 6),

	        new GameTickData(7, 0),
	        new RenderingData(0.6f, 7),
	        new GameTickData(7, 1),
	        new RenderingData(0.6f, 7),
	        new GameTickData(7, 2),
	        new RenderingData(0.6f, 7),
	        new GameTickData(7, 3),
	        new RenderingData(0.6f, 7),

	        new GameTickData(8, 0),
	        new RenderingData(0.6f, 8),
	        new GameTickData(8, 1),
	        new RenderingData(0.6f, 8),
	        new GameTickData(8, 2),
	        new RenderingData(0.6f, 8),
	        new GameTickData(8, 3),
	        new RenderingData(0.6f, 8),

	        new GameTickData(9, 0),
	        new RenderingData(0.6f, 9),
	        new GameTickData(9, 1),
	        new RenderingData(0.6f, 9),
	        new GameTickData(9, 2),
	        new RenderingData(0.6f, 9),
	        new GameTickData(9, 3),
	        new RenderingData(0.6f, 9),

	        new GameTickData(10, 0),
	        new RenderingData(0.6f, 10),
	        new GameTickData(10, 1),
	        new RenderingData(0.6f, 10),
	        new GameTickData(10, 2),
	        new RenderingData(0.6f, 10),
	        new GameTickData(10, 3),
	        new RenderingData(0.6f, 10),

			// Animation starts again
	        new GameTickData(1, 0),
	        new RenderingData(0.1f, 1),
	        new GameTickData(1, 1),
	        new RenderingData(0.6f, 1),
	        new GameTickData(1, 2),
	        new RenderingData(0.6f, 1),
	        new GameTickData(1, 3),
	        new RenderingData(0.6f, 1),
	    });
}
