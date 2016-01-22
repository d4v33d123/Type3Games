#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
namespace T3E
{
	class Camera2D
	{
	public:
		Camera2D();
		~Camera2D();

		void update();
		void init(int screenWidth, int screenHeight);

		//setters
		void setPosition(const glm::vec2& newPosition) { position_ = newPosition; needsMatrixUpdate_ = true; }
		void setScale(float newScale){ scale_ = newScale; needsMatrixUpdate_ = true; }

		//getters
		glm::vec2 getPosition() { return position_; }
		float getScale() { return scale_; }
		glm::mat4 getCameraMatrix() { return cameraMatrix_; }

	private:
		int screenWidth_;
		int screenHeight_;
		bool needsMatrixUpdate_;
		float scale_;
		glm::vec2 position_;
		glm::mat4 cameraMatrix_;
		glm::mat4 orthoMatrix_;
	};
}


