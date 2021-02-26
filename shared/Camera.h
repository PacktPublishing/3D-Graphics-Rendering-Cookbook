#pragma once

#include <assert.h>
#include <algorithm>

#include "shared/UtilsMath.h"
#include "shared/Trackball.h"

#include "glm/gtx/euler_angles.hpp"

class CameraPositionerInterface
{
public:
	virtual ~CameraPositionerInterface() = default;
	virtual glm::mat4 getViewMatrix() const = 0;
	virtual glm::vec3 getPosition() const = 0;
};

class Camera final
{
public:
	explicit Camera(CameraPositionerInterface& positioner)
		: positioner_(&positioner)
	{}

	Camera(const Camera&) = default;
	Camera& operator = (const Camera&) = default;

	glm::mat4 getViewMatrix() const { return positioner_->getViewMatrix(); }
	glm::vec3 getPosition() const { return positioner_->getPosition(); }

private:
	const CameraPositionerInterface* positioner_;
};

class CameraPositioner_FirstPerson final: public CameraPositionerInterface
{
public:
	CameraPositioner_FirstPerson() = default;
	CameraPositioner_FirstPerson(const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up)
	: cameraPosition_(pos)
	, cameraOrientation_(glm::lookAt(pos, target, up))
	, up_(up)
	{}

	void update(double deltaSeconds, const glm::vec2& mousePos, bool mousePressed)
	{
		if (mousePressed)
		{
			const glm::vec2 delta = mousePos - mousePos_;
			const glm::quat deltaQuat = glm::quat(glm::vec3(-mouseSpeed_ * delta.y, mouseSpeed_ * delta.x, 0.0f));
			cameraOrientation_ = deltaQuat * cameraOrientation_;
			cameraOrientation_ = glm::normalize(cameraOrientation_);
			setUpVector(up_);
		}
		mousePos_ = mousePos;

		const glm::mat4 v = glm::mat4_cast(cameraOrientation_);

		const glm::vec3 forward = -glm::vec3(v[0][2], v[1][2], v[2][2]);
		const glm::vec3 right = glm::vec3(v[0][0], v[1][0], v[2][0]);
		const glm::vec3 up = glm::cross(right, forward);

		glm::vec3 accel(0.0f);

		if (movement_.forward_) accel += forward;
		if (movement_.backward_) accel -= forward;

		if (movement_.left_) accel -= right;
		if (movement_.right_) accel += right;

		if (movement_.up_) accel += up;
		if (movement_.down_) accel -= up;

		if (movement_.fastSpeed_) accel *= fastCoef_;

		if (accel == glm::vec3(0))
		{
			// decelerate naturally according to the damping value
			moveSpeed_ -= moveSpeed_ * std::min((1.0f / damping_) * static_cast<float>(deltaSeconds), 1.0f);
		}
		else
		{
			// acceleration
			moveSpeed_ += accel * acceleration_ * static_cast<float>(deltaSeconds);
			const float maxSpeed = movement_.fastSpeed_ ? maxSpeed_ * fastCoef_ : maxSpeed_;
			if (glm::length(moveSpeed_) > maxSpeed) moveSpeed_ = glm::normalize(moveSpeed_) * maxSpeed;
		}

		cameraPosition_ += moveSpeed_ * static_cast<float>(deltaSeconds);
	}

	virtual glm::mat4 getViewMatrix() const override
	{
		const glm::mat4 t = glm::translate(glm::mat4(1.0f), -cameraPosition_);
		const glm::mat4 r = glm::mat4_cast(cameraOrientation_);
		return r * t;
	}

	virtual glm::vec3 getPosition() const override
	{
		return cameraPosition_;
	}

	void setPosition(const glm::vec3& pos)
	{
		cameraPosition_ = pos;
	}

	void resetMousePosition(const glm::vec2& p) { mousePos_ = p; };

	void setUpVector(const glm::vec3& up)
	{
		const glm::mat4 view = getViewMatrix();
		const glm::vec3 dir = -glm::vec3(view[0][2], view[1][2], view[2][2]);
		cameraOrientation_ = glm::lookAt(cameraPosition_, cameraPosition_ + dir, up);
	}

	inline void lookAt(const glm::vec3& pos, const glm::vec3& target, const glm::vec3& up) {
		cameraPosition_ = pos;
		cameraOrientation_ = glm::lookAt(pos, target, up);
	}

public:
	struct Movement
	{
		bool forward_ = false;
		bool backward_ = false;
		bool left_ = false;
		bool right_ = false;
		bool up_ = false;
		bool down_ = false;
		//
		bool fastSpeed_ = false;
	} movement_;

public:
	float mouseSpeed_ = 4.0f;
	float acceleration_ = 150.0f;
	float damping_ = 0.2f;
	float maxSpeed_ = 10.0f;
	float fastCoef_ = 10.0f;

private:
	glm::vec2 mousePos_ = glm::vec2(0);
	glm::vec3 cameraPosition_ = glm::vec3(0.0f, 10.0f, 10.0f);
	glm::quat cameraOrientation_ = glm::quat(glm::vec3(0));
	glm::vec3 moveSpeed_ = glm::vec3(0.0f);
	glm::vec3 up_ = glm::vec3(0.0f, 0.0f, 1.0f);
};

class CameraPositioner_MoveTo final : public CameraPositionerInterface
{
public:
	CameraPositioner_MoveTo(const glm::vec3& pos, const glm::vec3& angles)
		: positionCurrent_(pos)
		, positionDesired_(pos)
		, anglesCurrent_(angles)
		, anglesDesired_(angles)
	{}

	void update(float deltaSeconds, const glm::vec2& mousePos, bool mousePressed)
	{
		positionCurrent_ += dampingLinear_ * deltaSeconds * (positionDesired_ - positionCurrent_);

		// normalization is required to avoid "spinning" around the object 2pi times
		anglesCurrent_ = clipAngles(anglesCurrent_);
		anglesDesired_ = clipAngles(anglesDesired_);

		// update angles
		anglesCurrent_ -= angleDelta(anglesCurrent_, anglesDesired_) * dampingEulerAngles_ * deltaSeconds;

		// normalize new angles
		anglesCurrent_ = clipAngles(anglesCurrent_);

		const glm::vec3 a = glm::radians(anglesCurrent_);

		currentTransform_ = glm::translate(glm::yawPitchRoll(a.y, a.x, a.z), -positionCurrent_);
	}

	void setPosition(const glm::vec3& p) { positionCurrent_ = p; }
	void setAngles(float pitch, float pan, float roll) { anglesCurrent_ = glm::vec3(pitch, pan, roll); }
	void setAngles(const glm::vec3& angles) { anglesCurrent_ = angles; }
	void setDesiredPosition(const glm::vec3& p) { positionDesired_ = p; }
	void setDesiredAngles(float pitch, float pan, float roll) { anglesDesired_ = glm::vec3(pitch, pan, roll); }
	void setDesiredAngles(const glm::vec3& angles) { anglesDesired_ = angles; }

	virtual glm::vec3 getPosition() const override { return positionCurrent_; }
	virtual glm::mat4 getViewMatrix() const override { return currentTransform_; }

public:
	float dampingLinear_ = 10.0f;
	glm::vec3 dampingEulerAngles_ = glm::vec3(5.0f, 5.0f, 5.0f);

private:
	glm::vec3 positionCurrent_ = glm::vec3(0.0f);
	glm::vec3 positionDesired_ = glm::vec3(0.0f);

	/// pitch, pan, roll
	glm::vec3 anglesCurrent_ = glm::vec3(0.0f);
	glm::vec3 anglesDesired_ = glm::vec3(0.0f);

	glm::mat4 currentTransform_ = glm::mat4(1.0f);

	static inline float clipAngle(float d)
	{
		if (d < -180.0f) return d + 360.0f;
		if (d > +180.0f) return d - 360.f;
		return d;
	}

	static inline glm::vec3 clipAngles(const glm::vec3& angles)
	{
		return glm::vec3(
			std::fmod(angles.x, 360.0f),
			std::fmod(angles.y, 360.0f),
			std::fmod(angles.z, 360.0f)
		);
	}

	static inline glm::vec3 angleDelta(const glm::vec3& anglesCurrent, const glm::vec3& anglesDesired)
	{
		const glm::vec3 d = clipAngles(anglesCurrent) - clipAngles(anglesDesired);
		return glm::vec3(clipAngle(d.x), clipAngle(d.y), clipAngle(d.z));
	}
};
