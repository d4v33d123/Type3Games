#ifndef MAINGAME_H
#define MAINGAME_H

//SDL includes
#include "SDL.h"
//OpenGL ES includes
#ifdef __ANDROID__
	#include <GLES2/gl2.h>
	#include <GLES2/gl2ext.h>
	#endif  //__ANDROID__
//GLM includes
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"
//STL includes
#include <string>
#include <vector>
//Type3Engine includes
#include "Type3Engine/Sprite.h"
#include "Type3Engine/GLSLProgram.h"
#include "Type3Engine/glTexture.h"
#include "Type3Engine/window.h"
#include "Type3Engine/Type3Engine.h"
#include "Type3Engine/errors.h"
#include "Type3Engine/Camera.h"
#include "Type3Engine/AudioEngine.h"
//game classes
#include "Cell.h"
#include "BloodVessel.h"
#include "Grid.h"



#include "Type3Engine/Vertex.h"//draw grid

enum class GameState {PLAY, EXIT};

class MainGame
{
public:
	MainGame();
	~MainGame();

	void run();

private:
	//camera sensitivity
	const int PAN_SENSITIVITY;
	const int ZOOM_SENSITIVITY;
	// Debug Cursor
    glm::vec4 cursor_pos_;

	//control functions
	void initSystems();
	void initShaders();
	void gameLoop();
	void processInput(float dTime);
	void renderGame();
	void calculateFPS();
	
	//control vars
	int screenWidth_;
	int screenHeight_;
	float time_;
	float fps_;
	float frameTime_;
	float maxFPS_;
	T3E::window window_;
	GameState gameState_;
		
    T3E::Grid grid_;

	//GAMEPLAY
	Uint8 nOfFingers_;                  // n of fingers currently touching screen
	T3E::Camera camera_;                // 2d camera
	std::vector<T3E::Sprite*> sprites_; // sprite container

	// [in] row to test
	// [in] column to test
	// [ret] true if a bv was created, false otherwise
	// try to create a cell or blood vessel at the specified position
	// TODO: SHOULD THIS GO INTO GRID CLASS?
    bool growBloodVesselAt( int row, int col );
	
	// [in] row to test
	// [in] column to test
	// [ret] true if successfully selected a cell, false otherwise
	bool selectCell(int row, int col);
	
    //INPUT
	bool finger_dragged_;
	//detect when finger is down for a certain amount of time
	bool fingerPressed_;
	glm::vec2 pressPos_;
	float pressTimer_;
	//detect cell selection
	glm::vec2 selectedPos_;
	bool cellSelected_;
	
	//GRAPHICS
	T3E::GLSLProgram cellProgram_;//shader programs
	GLint cell_finalM_location, sampler0_location, inputColour_location; // shader uniform locations
	glm::mat4 worldM_, viewM_, projectionM_, viewProjInverse; // transform matrices
	glm::mat4 finalM_; // product of above 3, do in cpu once per geometry vs do in gpu once per each vertex(profile this?)	

	//AUDIO (not quite as important as graphics but more important than conversion functions)
    T3E::AudioEngine audioEngine_;
	
	// Conversion Functions
     // Returns a vec4 where x and y are the touch world positions, z is 0.0f, w is a number
    glm::vec4 touch_to_world( glm::vec2 touch_coord );
    // Returns an SDL_Point where x represents the row and y represents the column
    SDL_Point world_to_grid( glm::vec4 world_coord );
	
	//Draw hex grid 	
	T3E::GLSLProgram hexProgram_;
	GLint hex_finalM_location, hex_inputColour_location;
 	GLuint hexBufferName;
	T3E::Vertex hexVertexes[12];
	void drawGrid();
};

#endif
