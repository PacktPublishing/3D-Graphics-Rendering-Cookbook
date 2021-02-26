#pragma once

#include <assert.h>
#include <stdio.h>

class FramesPerSecondCounter
{
public:
	explicit FramesPerSecondCounter(float avgInterval = 0.5f)
	:avgInterval_(avgInterval)
	{
		assert(avgInterval > 0.0f);
	}

	bool tick(float deltaSeconds, bool frameRendered = true)
	{
		if (frameRendered)
			numFrames_++;

		accumulatedTime_ += deltaSeconds;

		if (accumulatedTime_ > avgInterval_)
		{
			currentFPS_ = static_cast<float>(numFrames_ / accumulatedTime_);
			if (printFPS_)
				printf("FPS: %.1f\n", currentFPS_);
			numFrames_ = 0;
			accumulatedTime_ = 0;
			return true;
		}

		return false;
	}

	inline float getFPS() const { return currentFPS_; }

	bool printFPS_ = true;

private:
	const float avgInterval_ = 0.5f;
	unsigned int numFrames_ = 0;
	double accumulatedTime_ = 0;
	float currentFPS_ = 0.0f;
};
