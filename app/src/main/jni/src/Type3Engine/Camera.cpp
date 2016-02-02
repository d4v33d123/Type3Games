#include "Camera.h"

namespace T3E
{
	void Camera::init(glm::vec3 position, glm::vec3 lookAt)
	{
		position_ = position;
		lookAt_ = lookAt;
		
		up_ = glm::vec3(0.0f,1.0f,0.0f);//up is +y axis
		zoomRange_ = glm::vec2(0.4f, 5.0f);	
		moveSensitivity_ = 2.0f;
		zoomSensitivity_ = 1.0f;
	}

	void Camera::moveDelta(glm::vec3 xyz)
	{
		position_ += xyz * moveSensitivity_ * position_.z;//more zoomed in -> move slower
		lookAt_.x = position_.x;
		lookAt_.y = position_.y;
	}

	void Camera::zoom(float distance)
	{
		//zoom if in zoomRange
		float delta = distance * zoomSensitivity_;
		if(((position_.z + delta) >= zoomRange_.x) && ((position_.z + delta) <= zoomRange_.y))
			position_.z += delta;
	}
	
	void Camera::moveTo(glm::vec3 xyz)
	{
		position_ = xyz;
		lookAt_.x = position_.x;
		lookAt_.y = position_.y;
	}
}