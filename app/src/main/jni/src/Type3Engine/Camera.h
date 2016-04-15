#ifndef CAMERA_H
#define CAMERA_H

#include "glm/glm.hpp"

namespace T3E
{
	class Camera
	{
	public:
		/**
		* initialises the camera 
		*
		* @param [in] The starting position for the camera
		* @param [in] The starting position that the camera is looking at
		*/
		void init(glm::vec3 position, glm::vec3 lookAt);//init parameters
		
		/**
		* Moving the camera by the amount passed in
		*
		* @param [in] The values to update the xyz values of the camera
		*/
		void moveDelta(glm::vec3 xyz);//add xyz to current position and keep lookat straight ahead
		
		/**
		* Zooms the camera in or out by the distance passed in
		*
		* @param [in] the distance to zoom in or out by
		*/
		void zoom(float distance);//move by distance along z axis
		
		/**
		* Move the camera to the desired position 
		*
		* @param [in] The position for the camera to be moved to
		*/
		void moveTo(glm::vec3 xyz);
		
		//setters
		/**
		* Sets the cameras position 
		*
		* @param [in] the position to move the camera to
		*/
		void setPosition(glm::vec3 position) { position_ = position; };
		
		/**
		* Set the position the camera is looking at 
		*
		* @param [in] The position for the camera to look at
		*/
		void setLookAt(glm::vec3 lookAt) { lookAt_ = lookAt; };
		
		/**
		* Set the up vector of the camera 
		*
		* @param [in] the new up vector for the camera
		*/
		void setUp(glm::vec3 up) { up_ = up; };
		
		/**
		* Set the range that the camera can zoom in or out 
		*
		* @param [in] the range
		*/
		void setZoomRange(glm::vec2 zr) { zoomRange_ = zr; };
		
		/**
		* Sets the sensitivity of the camera, allowing for quicker/slower panning/zooming 
		*
		* @param [in] The movement sensitivity
		* @param [in] The zoom sensitivity
		*/
		void setSensitivity(float move, float zoom) { moveSensitivity_ = move; zoomSensitivity_ = zoom;};
		
		//getters
		/**
		* Gets the Cameras current position
		*
		* @return [out] the cameras position
		*/
		glm::vec3 getPosition(){ return position_; };
		
		/**
		* Gets the position that the camera is currently looking at
		*
		* @return [out] the position the camera is looking at
		*/
		glm::vec3 getLookAt(){ return lookAt_; };
		
		/**
		* Gets the cameras current up vector
		*
		* @return [out] the cameras up vector
		*/
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