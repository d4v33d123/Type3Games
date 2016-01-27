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
#include "Type3Engine/Camera.h"
#include "Type3Engine/Cell.h"
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
	GameState gameState_;	
	int screenWidth_;
	int screenHeight_;
	float time_;
	float fps_;
	float frameTime_;
	float maxFPS_;
	Uint8 nOfFingers_;//n of fingers currently touching screen
	
	//sprite container
	std::vector<T3E::Sprite*> sprites_;
	//grid
	int rows;
	int columns;
	std::vector<T3E::Cell> grid_;//make this a vector of pointers if dynamically resizing?
	std::vector<T3E::Cell*> cells_;//living cells
	//shader programs
	T3E::GLSLProgram cellProgram_, triangleProgram_;
	//shader uniform locations
	GLint cell_finalM_location, triangle_finalM_location, sampler0_location, inputColour_location;	
	//transform matrices
	glm::mat4 worldM_, viewM_, projectionM_;
	glm::mat4 finalM_;//product of above 3, do in cpu once per geometry vs do in gpu once per each vertex(profile this)
	//2d camera
	T3E::Camera camera_;
 
	//TEMPORARY TESTING STUFF
	//triangle geometry
/* 	GLuint triangleBufferName;
	T3E::Vertex triangle[3];
	float parallX, parallY;//parallax effect test*/
};

#endif
