#ifndef MAINGAME_H
#define MAINGAME_H

//SDL includes
#include "SDL.h"
//OpenGL ES includes
#ifdef __ANDROID__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
	#endif//__ANDROID__
//GLM includes
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
//Type3Engine includes
#include "Type3Engine/Sprite.h"
#include "Type3Engine/GLSLProgram.h"
#include "Type3Engine/glTexture.h"
#include "Type3Engine/window.h"
#include "Type3Engine/Type3Engine.h"
#include "Type3Engine/errors.h"
//STL includes
#include <string>
#include <vector>

//test triangle
#include "Type3Engine/Vertex.h"

enum class GameState {PLAY, EXIT};

class MainGame
{
public:
	MainGame();
	~MainGame();

	void run();

private:
	
	void initSystems();
	void initShaders();
	void gameLoop();
	void processInput();
	void renderGame();
	void calculateFPS();

	T3E::window window_;
	int screenWidth_;
	int screenHeight_;
	GameState gameState_;

	std::vector<T3E::Sprite*> sprites_;
	
	T3E::GLSLProgram cellProgram_, triangleProgram_;
	
	float time_;
	float fps_;
	float frameTime_;
	float maxFPS_;
	
	//geometry etc
	GLuint triangleBufferName;
	T3E::Vertex triangle[3];
	float parallX, parallY;//parallax effect test
	//shaders
	//uniforms locations
	GLint cell_finalM_location, triangle_finalM_location, sampler0_location, inputColour_location;
	//transform matrices, initialised as identity mattrix
	glm::mat4 worldM, viewM, projectionM;
	glm::mat4 finalM;	
	
	// TOUCH TEST STUFF	
	glm::vec3 camPos;
	glm::vec3 lookatPos;
	int fingers;
	glm::vec4 worldPos;
	//END TOUCH TEST STUFF
};

#endif
