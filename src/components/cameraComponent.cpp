#include "components/cameraComponent.hpp"

#include "components/component.hpp"
#include "game.hpp"
#include "third_party/Eigen/src/Core/Matrix.h"
#include "third_party/Eigen/src/Geometry/AngleAxis.h"
#include "third_party/Eigen/src/Geometry/Quaternion.h"
#include "utils.hpp"

#include <SDL3/SDL.h>
#include <cmath>

CameraComponent::CameraComponent(Actor* owner, int priority)
	: Component(owner, priority), mFOV(45) {
	mProjectionMatrix = Eigen::Affine3f::Identity();

	Eigen::Quaternionf dir = mOwner->getRotation();
	Eigen::AngleAxisf rot(toRadians(90), Eigen::Vector3f::UnitY());
	mOwner->setRotation(dir * rot);
};

CameraComponent::~CameraComponent() {}

void CameraComponent::update(float delta) {
	float x, y;
	SDL_GetRelativeMouseState(&x, &y);

	Eigen::Quaternionf dir = mOwner->getRotation();
	Eigen::AngleAxisf rot(-(x / 50) * delta, Eigen::Vector3f::UnitY());
	Eigen::AngleAxisf yaw(-(y / 50) * delta, Eigen::Vector3f::UnitZ());
	// Eigen::Quaternionf newDir = dir * rot * yaw;
	// dir.w() += cos(toRadians(x) / 2);
	// dir.y() += sin(toRadians(x) / 2);
	mOwner->setRotation(dir * rot * yaw);

	project();
	view();

	mOwner->getGame()->mView = mViewMatrix;
	mOwner->getGame()->mProjection = mProjectionMatrix;

	(void)delta;
}

void CameraComponent::view() {
	Eigen::Matrix3f R;
	const Eigen::Vector3f up(0.0f, 1.0f, 0.0f);
	R.col(2) = (-mOwner->getForward() * 100.f).normalized();
	R.col(0) = up.cross(R.col(2)).normalized();
	R.col(1) = R.col(2).cross(R.col(0));
	mViewMatrix.matrix().topLeftCorner<3, 3>() = R.transpose();
	mViewMatrix.matrix().topRightCorner<3, 1>() = -R.transpose() * mOwner->getPosition();
	mViewMatrix(3, 3) = 1.0f;
}

void CameraComponent::project() {
	const float near = 0.1f;
	const float far = 100.0f;
	const float aspect =
		static_cast<float>(mOwner->getGame()->getWidth()) / mOwner->getGame()->getHeight();
	float theta = mFOV * 0.5;
	float range = far - near;
	float invtan = 1. / tan(theta);

	// https://www.songho.ca/opengl/gl_projectionmatrix.html
	mProjectionMatrix(0, 0) = invtan / aspect;
	mProjectionMatrix(1, 1) = invtan;
	mProjectionMatrix(2, 2) = -(near + far) / range;
	mProjectionMatrix(3, 2) = -1;
	mProjectionMatrix(2, 3) = -2 * near * far / range;
	mProjectionMatrix(3, 3) = 0;
}