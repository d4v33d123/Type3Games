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
//game classes
#include "Cell.h"
#include "BloodVessel.h"

//test triangle
//#include "Type3Engine/Vertex.h"

enum class GameState {PLAY, EXIT};

class MainGame
{
public:
	MainGame();
	~MainGame();

	void run();

private:
	//grid dimensions
	const int ROWS;
	const int COLUMNS;
	//camera sensiticity
	const int PAN_SENSITIVITY;
	const int ZOOM_SENSITIVITY;
	// Debug Cursor
    glm::vec4 cursor_pos_;

	//control functions
	void initSystems();
	void initShaders();
	void gameLoop();
	void processInput();
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
		
	//GAMEPLAY
	Uint8 nOfFingers_;//n of fingers currently touching screen
	T3E::Camera camera_;//2d camera
	std::vector<T3E::Sprite*> sprites_;//sprite container
	std::vector<T3E::Hex*> grid_;//hex grid with game elements
	std::vector<T3E::Cell*> cells_;//living cells
	std::vector<T3E::BloodVessel*> bloodVessels_;
    bool finger_dragged_;
	
	void createBloodVessel(int row, int column);
	
	//GRAPHICS
	T3E::GLSLProgram cellProgram_;//shader programs
	GLint cell_finalM_location, sampler0_location, inputColour_location;//shader uniform locations
	glm::mat4 worldM_, viewM_, projectionM_, viewProjInverse;//transform matrices
	glm::mat4 finalM_;//product of above 3, do in cpu once per geometry vs do in gpu once per each vertex(profile this?)	

    // Conversion Functions
    
    // Returns a vec4 where x and y are the touch world positions, z is 0.0f, w is a number
    glm::vec4 touch_to_world( glm::vec2 touch_coord );
    // Returns an SDL_Point where x represents the row and y represents the column
    SDL_Point world_to_grid( glm::vec4 world_coord );

	/*CODE FOR TRIANGLES WITH PARALLAX
	T3E::GLSLProgram triangleProgram_;
	GLint triangle_finalM_location;
 	GLuint triangleBufferName;
	T3E::Vertex triangle[3];
	float parallX, parallY;//parallax effect test*/
};

#endif
