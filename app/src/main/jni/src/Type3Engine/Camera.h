#ifndef CAMERA_H
#define CAMERA_H

#include "glm/glm.hpp"

namespace T3E
{
	class Camera
	{
	public:
		void init(glm::vec3 position, glm::vec3 lookAt);//init parameters
		void moveBy(glm::vec3 xyz);//add xyz*sensitivity to current position and keep lookat straight ahead
		void zoom(float distance);//move by distance along z axis
		void moveTo(glm::vec3 xyz);
		
		//setters
		void setPosition(glm::vec3 position) { position_ = position; };
		void setLookAt(glm::vec3 lookAt) { lookAt_ = lookAt; };
		void setUp(glm::vec3 up) { up_ = up; };
		void setZoomRange(glm::vec3 up) { up_ = up; };
		void setSensitivity(float move, float zoom) { moveSensitivity_ = move; zoomSensitivity_ = zoom;};
		
		//getters
		glm::vec3 getPosition(){ return position_; };
		glm::vec3 getLookAt(){ return lookAt_; };
		glm::vec3 getUp(){ return up_; };
		
	private:
		glm::vec3 position_;
		glm::vec3 lookAt_;
		glm::vec3 up_;
		glm::vec2 zoomRange_;//max and min pos on z axis
		float moveSensitivity_, zoomSensitivity_;//TODO: CONTROL THIS IN GAME MENU OPTIONS
	};
}

#endif