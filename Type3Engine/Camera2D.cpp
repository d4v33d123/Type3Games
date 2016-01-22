#include "Camera2D.h"

namespace T3E
{
	Camera2D::Camera2D() : position_(0.0f, 0.0f),
		cameraMatrix_(1.0f),
		orthoMatrix_(1.0f),
		scale_(1.0f),
		needsMatrixUpdate_(true),
		screenWidth_(500),
		screenHeight_(500)
	{
	}


	Camera2D::~Camera2D()
	{
	}

	void Camera2D::init(int screenWidth, int screenHeight)
	{
		screenWidth_ = screenWidth;
		screenHeight_ = screenHeight;
		orthoMatrix_ = glm::ortho(0.0f, (float)screenWidth_, 0.0f, (float)screenHeight_);
	}

	void Camera2D::update()
	{
		if (needsMatrixUpdate_)
		{
			//translate
			glm::vec3 translate(-position_.x + screenWidth_/2, -position_.y + screenHeight_/2, 0.0f);
			cameraMatrix_ = glm::translate(orthoMatrix_, translate);

			//scale
			glm::vec3 scale(scale_, scale_, 0.0f);
			cameraMatrix_ = glm::scale(glm::mat4(1.0f), scale) * cameraMatrix_;

			needsMatrixUpdate_ = false;
		}
	}
}

